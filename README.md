# Operating Systems Hands-On Projects

A collection of hands-on Operating Systems projects implemented in **C** on Linux.

The projects cover core systems-programming concepts including process management, multithreading and synchronization, Linux kernel programming, character-device drivers, virtual memory, page tables, TLBs, and low-level memory management.

## Projects

### 1. Unix Shell

A simple Unix-style shell implemented in C.

The shell reads and parses user commands, creates child processes using `fork()`, executes external programs using `execvp()`, and supports both foreground and background execution.

It also implements command history and basic shell-specific commands.

**Key concepts:**

* Linux processes
* `fork()` / `execvp()` / `wait()`
* Foreground and background execution
* Command parsing
* File descriptors
* Process management

[View project](./hw1-unix-shell)

---

### 2. Concurrent Linked List

A thread-safe sorted linked list implemented using **POSIX threads (`pthreads`)**.

Each node contains its own mutex, allowing the implementation to use fine-grained **hand-over-hand locking** rather than locking the entire list.

This enables multiple threads to safely operate on independent parts of the linked list concurrently.

**Key concepts:**

* Multithreading
* POSIX threads
* Mutexes
* Race conditions
* Synchronization
* Fine-grained locking
* Hand-over-hand locking
* Dynamic memory management

[View project](./hw2-concurrent-linked-list)

---

### 3. Linux Character Device Driver

A Linux character-device driver implemented in C as a **kernel module**.

The driver simulates two encryption devices: a Caesar cipher device and an XOR cipher device.

It implements Linux device operations including `open`, `read`, `write`, `release`, and `ioctl`, while managing communication between user space and kernel space.

**Key concepts:**

* Linux kernel modules
* Character-device drivers
* User space vs kernel space
* `file_operations`
* `open`, `read`, `write`, `release`, `ioctl`
* `copy_from_user()` / `copy_to_user()`
* `kmalloc()` / `kfree()`
* Major and minor device numbers
* File descriptors

[View project](./hw3-linux-device-driver)

---

### 4. Virtual Memory Translator

A virtual-memory simulation that translates virtual addresses into physical addresses.

The implementation uses a page table and a fixed-size TLB to cache address translations. When a page is not currently mapped, the program handles the page fault by loading the required page from a backing store into simulated physical memory.

**Key concepts:**

* Virtual memory
* Virtual-to-physical address translation
* Page tables
* Translation Lookaside Buffer (TLB)
* Page faults
* Demand paging
* Backing store
* `mmap()`
* Bitwise shifts and masks

[View project](./hw4-virtual-memory)

## Technologies

* C
* Linux
* GCC
* POSIX Threads
* Linux Kernel Modules
* Git
* Make
* GDB / Valgrind

## Topics Covered

* Processes and system calls
* Multithreading and synchronization
* Race conditions and mutexes
* Kernel-space programming
* Device-driver interfaces
* Dynamic memory management
* Virtual memory and paging
* TLB caching
* Bitwise operations
* Debugging and memory safety
