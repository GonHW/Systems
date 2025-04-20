FALL 2022
01:198:214
Professor Ames

Assignment 4: mymalloc.c mymalloc.h

Group names: 
Hench Wu		(NetID: hhw14) 
Jonathan Davies	(NetID: jpd244) 
Yiming Huang	(NetID: yh714)

we compile the test file like this:
gcc -g -Wall -Werror -fsanitize=address mymalloc.c test.c -o test

Description
This project implements a custom memory allocator with functionality similar to the standard malloc, free, and realloc. It simulates a heap space of 1MB and uses an explicit free list to manage memory blocks. The allocator supports three memory allocation strategies:

First Fit

Next Fit

Best Fit

Features
myinit(int allocAlg)
Initializes a 1MB heap and sets the allocation algorithm (0 = first fit, 1 = next fit, 2 = best fit).

mymalloc(size_t size)
Allocates memory of the specified size from the simulated heap using the selected fit strategy.

myfree(void ptr)*
Frees the memory at the given pointer and coalesces adjacent free blocks for efficient reuse.

myrealloc(void ptr, size_t size)*
Reallocates memory, expanding or shrinking the block as needed. If reallocation fails in-place, data is copied to a new region.

mycleanup()
Releases all resources by freeing the simulated heap.

printFreeList()
(Debug only) Prints the size of each free block in the free list.

Notes
All returned memory is 8-byte aligned.

An explicit free list is used to track free memory blocks.

Coalescing is done both on the left and right sides during free.

Uses memcpy during realloc to preserve data.

Compilation
Compile using:

bash
Copy
Edit
make all
Recommended flags (used in the Makefile):

bash
Copy
Edit
-g -Wall -Wvla -fsanitize=address
File Structure
Copy
Edit
hw4/
├── Makefile
├── mymalloc.c
├── mymalloc.h
└── README.txt
