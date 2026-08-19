# Linux Character Device Driver

A Linux character-device driver implemented in **C** as a kernel module for an Operating Systems hands-on assignment.

The project simulates two encryption devices:

- Caesar cipher device
- XOR cipher device

The main goal was to understand how user-space programs interact with devices through the Linux kernel and how a device driver exposes operations such as `open`, `read`, `write`, and `ioctl`.

## How It Works

The kernel module registers a character-device driver and manages two device buffers, one for the Caesar device and one for the XOR device.

The two simulated devices are distinguished using minor numbers:

- Minor `0` → Caesar device
- Minor `1` → XOR device

Each device supports standard character-device operations through Linux `file_operations`.

## Device Operations

The driver implements:

- `open()` — selects the appropriate device and allocates per-open private state
- `release()` — frees the private state associated with the open file
- `write()` — copies data from user space, encrypts it, and stores it in the device buffer
- `read()` — returns either raw encrypted data or decrypted data
- `ioctl()` — changes device-specific settings such as encryption key and read mode

## User Space and Kernel Space

Because the driver runs in kernel space, data is transferred safely across the user/kernel boundary using:

- `copy_from_user()` for writes
- `copy_to_user()` for reads

Kernel memory is allocated and released using:

- `kmalloc()`
- `kfree()`

## Per-File State

Each open file keeps its own:

- Encryption key
- Read mode

This state is stored using the file object's `private_data`.

The underlying device buffer is shared by all file descriptors that refer to the same device.

## Encryption

The project simulates two encryption methods:

### Caesar Cipher

Each character is shifted using an encryption key.

### XOR Cipher

Each character is combined with the key using the bitwise XOR operator.

## Key Concepts

- C programming
- Linux kernel modules
- Character-device drivers
- User space vs kernel space
- `file_operations`
- `open`, `read`, `write`, `release`, `ioctl`
- `copy_from_user()` / `copy_to_user()`
- `kmalloc()` / `kfree()`
- Major and minor device numbers
- File descriptors
- Kernel memory management
- Bitwise XOR