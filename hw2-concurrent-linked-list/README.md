# Concurrent Linked List

A thread-safe sorted linked list implemented in **C** using the POSIX threads library (`pthreads`) as part of an Operating Systems hands-on assignment.

The goal of the project was to allow multiple threads to access and modify the same linked list concurrently while preventing race conditions and avoiding a single global lock for the entire list.

## Features

- Sorted linked-list implementation
- Thread-safe insertion and removal
- Concurrent list traversal
- Per-node mutex synchronization
- Hand-over-hand locking
- Counting elements using a predicate function
- Swapping values
- Dynamic memory allocation and cleanup

## How It Works

Each node in the list stores:

- An integer value
- A pointer to the next node
- A `pthread_mutex_t` lock

Instead of locking the entire linked list, the implementation uses **fine-grained locking**. During traversal, the thread locks neighboring nodes and releases the previous lock as it moves forward.

This technique, known as **hand-over-hand locking**, allows different threads to safely work on separate parts of the linked list at the same time.

For example, one thread can modify nodes near the beginning of the list while another thread works on nodes farther away, as long as their locked regions do not overlap.

## Memory Management

Nodes are dynamically allocated using `malloc()`.

Each node's mutex is initialized when the node is created. When a node is removed or the list is deleted, its mutex is destroyed and the allocated memory is released using `free()`.

## Key Concepts

- C programming
- POSIX threads (`pthreads`)
- Multithreading
- Mutexes
- Race conditions
- Synchronization
- Fine-grained locking
- Hand-over-hand locking
- Linked lists
- Dynamic memory management