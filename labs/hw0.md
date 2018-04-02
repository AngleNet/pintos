### 1. objdump -x -d wc
* What  file format is used for this binary?  
    > elf64-x86-64
* What architecture is it compiled for?
    > i836:x86-64
* What are some of the names of segment/sections you  find?
    > .text .rodata .data .init .bss .comment .debug_info .debug_line .debug_str
* What segment/section contains main(the function) and what is the address of main?(It should be the same as what you saw in gdb)
    > .text segment. main@0x4004ed
* Do you see the stack segment anywhere?  What about the heap?  Explain.
    > No. Stack and heap are runtime concepts. They are created dynamically.

### 2. user limits
```
stack size: 7872
process limit: 8388608
max file descriptors: 1024
```
