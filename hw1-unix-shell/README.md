# Unix Shell

A simple Unix-style shell implemented in **C** as part of an Operating Systems hands-on assignment. The project explores Linux process creation and management by implementing a command-line shell that executes user commands and supports foreground and background processes.

## Features

- Reads and parses commands entered by the user
- Executes external programs using `fork()` and `execvp()`
- Supports foreground process execution using `wait()`
- Supports background execution with `&`
- Maintains a command history
- Provides built-in commands for displaying and clearing history
- Handles shell termination with an `exit` command

## How It Works

The shell continuously reads a command from the user and parses it into an argument array. For external commands, it creates a child process using `fork()`.

The child process uses `execvp()` to replace itself with the requested program, while the parent process remains the shell. For foreground commands, the parent waits for the child to finish. For commands ending with `&`, the shell treats the process as a background process and continues accepting commands without waiting.

## Key Concepts

- C programming
- Linux processes
- `fork()` / `execvp()` / `wait()`
- Foreground and background execution
- Command parsing
- Process management
- Dynamic memory management