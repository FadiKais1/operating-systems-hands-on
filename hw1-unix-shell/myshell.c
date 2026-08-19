#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 100
#define MAX_HISTORY 100
#define MAX_ARGS    50

/* ---- History storage ---- */
static char history[MAX_HISTORY][BUFFER_SIZE];
static int  history_count = 0;

static void history_add(const char *cmd)
{
    if (history_count < MAX_HISTORY)
    {
        strncpy(history[history_count], cmd, BUFFER_SIZE - 1);
        history[history_count][BUFFER_SIZE - 1] = '\0';
        history_count++;
    }
}

/* Print in reverse order with sequential numbers */
static void history_print(void)
{
    int i   = history_count - 1;
    int num = history_count;
    while (i >= 0)
    {
        fprintf(stdout, "%d %s\r\n", num, history[i]);
        i--;
        num--;
    }
}

static void history_clear(void)
{
    history_count = 0;
}

/* return 1 if the line is empty or only spaces/tabs/CR, 0 otherwise */
static int is_blank(const char *s)
{
    while (*s != '\0')
    {
        if (*s != ' ' && *s != '\t' && *s != '\r')
            return 0;
        s++;
    }
    return 1;
}

int main(void)
{
    close(2);
    dup(1);

    char command[BUFFER_SIZE];
    char original[BUFFER_SIZE];

    while (1)
    {
        fprintf(stdout, "my-shell> ");
        memset(command, 0, BUFFER_SIZE);

        /* stop if input ends (EOF / Ctrl-D) so we don't loop forever */
        if (fgets(command, BUFFER_SIZE, stdin) == NULL)
            break;

        /* strip trailing newline / carriage return (handles \n and \r\n) */
        int len = strlen(command);
        while (len > 0 && (command[len - 1] == '\n' || command[len - 1] == '\r'))
        {
            command[len - 1] = '\0';
            len--;
        }

        /* skip empty or whitespace-only lines */
        if (is_blank(command))
            continue;

        /* exit */
        if (strncmp(command, "exit", 4) == 0)
            break;

        /* save the command exactly as typed (with & if present) for history */
        strncpy(original, command, BUFFER_SIZE - 1);
        original[BUFFER_SIZE - 1] = '\0';

        /* split the command into tokens by whitespace */
        char *argv[MAX_ARGS];
        int   argc = 0;
        char *tok = strtok(command, " \t");
        while (tok != NULL && argc < MAX_ARGS - 1)
        {
            argv[argc++] = tok;
            tok = strtok(NULL, " \t");
        }
        argv[argc] = NULL;

        if (argc == 0)
            continue;

        /* clear_history (also accept history_clear spelling).
           it is enough that the first word starts with the command name.
           checked BEFORE history, since "history_clear" also starts with "history". */
        if (strncmp(argv[0], "clear_history", 13) == 0 ||
            strncmp(argv[0], "history_clear", 13) == 0)
        {
            history_clear();
            continue;
        }

        /* every other command (including failed ones) goes into the history */
        history_add(original);

        /* history: enough that the first word starts with "history" (note 9) */
        if (strncmp(argv[0], "history", 7) == 0)
        {
            history_print();
            continue;
        }

        /* background: last token ends with '&' */
        int background = 0;
        char *last = argv[argc - 1];
        int   last_len = strlen(last);
        if (last_len > 0 && last[last_len - 1] == '&')
        {
            background = 1;
            last[last_len - 1] = '\0';
            if (strlen(last) == 0)      /* the '&' was its own word */
                argv[--argc] = NULL;
        }

        /* fork and run the command */
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("error");
        }
        else if (pid == 0)
        {
            execvp(argv[0], argv);
            perror("error");
            exit(1);
        }
        else
        {
            if (!background)
                wait(NULL);
        }
    }

    return 0;
}
