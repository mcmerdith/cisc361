# Programming Assignment 2

Tasty Linked List (Linus Torvalds)

## Makefile

Build with `make`

## Instructions

1. Implement `delete(int data)`

>Function should remove ALL entires in the list that match the same integer data value input by the user. Modify the "ASCII user interface" to allow uesr to input the integer "data" value of the entries that shall be deleted

2. Modify the `Makefile` 
>Compile `tastyDelete.c` and link its object file with other object files to create the exectuable `taste` when typing `make`

3. Name the program `tastyDelete.c`

>Make sure to use `valgrind` to check for memory leaks / illegal memory access and use `gdb` to debug your code
