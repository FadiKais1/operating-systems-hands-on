# Virtual Memory Translator

A virtual-memory simulation implemented in **C** as part of an Operating Systems hands-on assignment.

The project simulates the translation of virtual addresses into physical addresses using a page table and a small TLB cache, while also handling page faults and loading pages from a backing store.

## How It Works

For each virtual address, the program:

1. Splits the address into a virtual page number and an offset.
2. Checks the Translation Lookaside Buffer (TLB) for a cached translation.
3. If the translation is not in the TLB, checks the page table.
4. If the page is not currently mapped, handles a page fault by loading the page from the backing store into simulated physical memory.
5. Updates the page table and TLB.
6. Combines the physical page number with the original offset to produce the physical address.
7. Reads the corresponding value from simulated main memory.

## TLB

The TLB stores recently used virtual-to-physical page translations.

The implementation uses a fixed-size TLB with cyclic replacement, so once the TLB is full, new entries overwrite the oldest entries.

## Page Fault Handling

If a virtual page does not yet have a valid page-table entry, the program treats this as a page fault.

The required page is copied from the backing store into the next available physical page, and the page table is updated with the new mapping.

## Address Translation

The virtual address is divided into:

- Virtual page number
- Offset within the page

Bitwise shifts and masks are used to extract these fields.

Once the physical page is known, it is combined with the same offset to construct the final physical address.

## Statistics

The program tracks:

- Number of translated addresses
- Number of page faults
- Page fault rate
- Number of TLB hits
- TLB hit rate

## Key Concepts

- C programming
- Virtual memory
- Virtual-to-physical address translation
- Page tables
- Translation Lookaside Buffer (TLB)
- Page faults
- Demand paging
- Backing store
- Memory mapping with `mmap()`
- Bitwise shifts and masks
- Memory management