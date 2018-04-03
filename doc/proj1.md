## Project1 Threads

### Readings
* Introduction
    
    * Source tree overview
        ```
        threads: base kernel
        userprog: user program loader
        vm: virtual memory
        filesys: basic file system
        devices: I/O device interfacing
        lib: subset of standard library
        lib/kernel: included by kernel
        lib/user: included by user programs
        tests: test code
        examples: user program examples
        utils: pintos helpers
        ```
    * Building
        
        `cd threads && make ` will build the threads module. The object
        files are in `build` directory.
        ```
        kernel.o: the object file for the entire kernel
        kernel.bin: kernel.o with the debugging information stripped out.
        loader.bin: memory image of the kernel loader.
        ```
    * Running

        After building `threads` then `cd build && pintos --gdb -- run 
        alarm-multiple`. `pintos` has many useful flags, try it out. The 
        following is more useful:
        > -v: disable X output<br>
        > pintos run alarm-multiple > logfile: redirect bochs screen to /home/anglenet
        > logfile <br>
        > -t: use terminal as VGA <br>
        > -s: suppress serial input

    * Debugging

        One the most important thing is **reproducibility**. Bochs is featured by **jitter** which makes bochs timer interrupts came 
        at random intervals, but in a perfectly predictable way.
        > In particular, if you invoke pintos with the option -j seed, timer interrupts will come at irregularly spaced intervals. Within a single seed value, execution will still be reproducible, but timer behavior will change as seed is varied.
    
    * Testing

        Testing cases are in `tests` directory. `cd thread && make check` runs tests.
* Coding style
    * Style
        
        When you want to delete lines of code, do not put it into comments   or add conditional compiling directives, just remove it. Pintos tries to comply the [GNU Coding standards](http://www.gnu.org/prep/standards_toc.html).
    * C99

        > stdbool.h: macro bools<br>
        > stdint.h: intn_t, intptr_t<br>
        > stddef.h: size_t<br>
        > inttypes.h: format types. defines `PRI` macros<br>
    * Unsafe string function

        A few of the string functions declared in the standard <string.h> and <stdio.h> headers are notoriously unsafe.

        * strcpy
        * strncpy
        * strcat
        * strncat
        * strtok
        * sprintf
        * vsprintf

* Linker

    After we compiled all of the source code to object files, there may be some unresolved calls or variable locations still in it. Linking is the process to put the pieces together.  Each linking is controlled by a linker script. The main purpose of link scripts is:
    * how the input object files are mappred into the output file
    * controls the memory layout of the output file
        
            -T: use a new linker script
    
    1. Concepts
    > Each section should have two address: *VMA*: This is the address the section will have when the output file is run; *LMA*: This is the address at which the section will be loaded. You can see this via `objdump -h`

    2. Linker command
        * ENTRY(*symbol*): specifies the first instruction to execute
        * INCLUDE *filename*: include the linker script file
        * INPUT(*file, file*): input object file
        * OUTPUT(*file*): output file name
        * STARTUP(*file*): the first file to be linked
        * OUTPUTFORMAT(*bfdname*)
        * SECTIONS: `-M` to generate a map how the input section goes to the output section. The input sections of the same type will be merged into one section in the order of the input files on the command line.
    
    3. [GNU ld linker](ftp://ftp.gnu.org/old-gnu/Manuals/ld-2.9.1/html_mono/ld.html)

* Loader

    There are two different syntaxes about assembly code:
    * AT&T: opcode has a suffix to denote data types, use sigils, and place the destination operand on the right.
    * Intel: operands use a keyword to denote data type, no sigils, destination operand is leftmost.

    When the system starts up, CPU is reset to:

        IP: 0xfff0
        CS: 0xf000
        CS base: 0xffff 0000
    This is used until the CS is changed. CPU jumps to BIOS, BIOS does two things: POST and load the kernel loader and transfer control to kernel loader. The kernel loader start executing from 0x
    
* `objdump`

    > Disassemble 16-bit binary code

        objdump -S -mi386 -Maddr16,data16,att loader.o 

* IA-32 Architecture 
    * Paging
        >`CR3` is called *Page directory base register*. It can only be changed by a `movl IMME, %cr3` or by a task switch.

        > GDT: is a data structure in linear address space and used for all programs and tasks. LDT:  allowing each task to have a different logical-to-physical address mapping for LDT-based segments

        > In fact, the Paging is enabled via load the PDBR and set the specific flags in CR0.

    * How does CPU knows which code is kernel but others are not?
        
        This ring0 operation of kernel is configured by GDT when OS starts up. The CPU only executes whatever instructions it has. The OS will change the previlege in the system call. The system call is implemented via interrupt handler, the interrupt handler is configured to have the ring0 previlege.

    * Procedure calls, Interupts, Exceptions

        Operating system rely on stacks to make procedure calls, run interupt service routines and handling exceptions. A stack is a contiguous area of memory address which identified by a segment selector in the *SS* register. The head of the stack is kept in *ESP* register. The maximum number of stacks in the system is limited by the number of segments and the amount of physical memory. Only one stack is ativated a time in the system. The *EBP* is typically set to point to the return instruction pointer. The stack pointer for a stack segment should be aligned on 16-bit or 32-bit. The D flag in the segment descriptor for the current code segment sets the stack-segment width.
* Codes
    ```
    mov $es:0x18 %dx
    mov %dx start
    movw 0x2000 start+2
    ```
    >comes from `loader.S`

* Others

    > Python: `'{0:b}'.format(2)'` shows 2 in binary format.
        
### Design
#### 1. Efficient sleep 
* Option 1: 在timer的中断处理程序中查看到期的thread,然后唤醒
* Option 2: 在timer

#### PRELIMINARIES

> If you have any preliminary comments on your submission, notes for the
> TAs, or extra credit, please give them here.
*  [Art of Assembly](https://courses.engr.illinois.edu/ece390/books/artofasm/)
    Book of assembly code.

* [Bochs internal debugger](http://bochs.sourceforge.net/doc/docbook/user/internal-debugger.html)

* [Nice assembly language guaid](http://flint.cs.yale.edu/cs421/papers/x86-asm/asm.html)

* [Nice IA32 assembly programming guaid](http://flint.cs.yale.edu/cs422/doc/pc-arch.html)

> Please cite any offline or online sources you consulted while
> preparing your submission, other than the Pintos documentation, course
> text, lecture notes, and course staff.

#### ALARM CLOCK

* DATA STRUCTURES 

> A1: Copy here the declaration of each new or changed `struct' or
> `struct' member, global or static variable, `typedef', or
> enumeration.  Identify the purpose of each in 25 words or less.

* ALGORITHMS 

> A2: Briefly describe what happens in a call to timer_sleep(),
> including the effects of the timer interrupt handler.

> A3: What steps are taken to minimize the amount of time spent in
> the timer interrupt handler?

* SYNCHRONIZATION 

> A4: How are race conditions avoided when multiple threads call
> timer_sleep() simultaneously?

> A5: How are race conditions avoided when a timer interrupt occurs
> during a call to timer_sleep()?

* RATIONALE 

> A6: Why did you choose this design?  In what ways is it superior to
> another design you considered?

#### PRIORITY SCHEDULING

* DATA STRUCTURES 

> B1: Copy here the declaration of each new or changed `struct' or
> `struct' member, global or static variable, `typedef', or
> enumeration.  Identify the purpose of each in 25 words or less.

> B2: Explain the data structure used to track priority donation.
> Use ASCII art to diagram a nested donation.  (Alternately, submit a
> .png file.)

* ALGORITHMS 

> B3: How do you ensure that the highest priority thread waiting for
> a lock, semaphore, or condition variable wakes up first?

> B4: Describe the sequence of events when a call to lock_acquire()
> causes a priority donation.  How is nested donation handled?

> B5: Describe the sequence of events when lock_release() is called
> on a lock that a higher-priority thread is waiting for.

* SYNCHRONIZATION

> B6: Describe a potential race in thread_set_priority() and explain
> how your implementation avoids it.  Can you use a lock to avoid
> this race?

* RATIONALE

> B7: Why did you choose this design?  In what ways is it superior to
> another design you considered?

#### ADVANCED SCHEDULER

* DATA STRUCTURES ----

> C1: Copy here the declaration of each new or changed `struct' or
> `struct' member, global or static variable, `typedef', or
> enumeration.  Identify the purpose of each in 25 words or less.

* ALGORITHMS ----

> C2: Suppose threads A, B, and C have nice values 0, 1, and 2.  Each
> has a recent_cpu value of 0.  Fill in the table below showing the
> scheduling decision and the priority and recent_cpu values for each
> thread after each given number of timer ticks:
```
imer  recent_cpu    priority   thread
icks   A   B   C   A   B   C   to run
----  --  --  --  --  --  --   ------
0
4
8
2
6
0
4
8
2
6
```

> C3: Did any ambiguities in the scheduler specification make values
> in the table uncertain?  If so, what rule did you use to resolve
> them?  Does this match the behavior of your scheduler?

> C4: How is the way you divided the cost of scheduling between code
> inside and outside interrupt context likely to affect performance?

* RATIONALE 

> C5: Briefly critique your design, pointing out advantages and
> disadvantages in your design choices.  If you were to have extra
> time to work on this part of the project, how might you choose to
> refine or improve your design?

> C6: The assignment explains arithmetic for fixed-point math in
> detail, but it leaves it open to you to implement it.  Why did you
> decide to implement it the way you did?  If you created an
> abstraction layer for fixed-point math, that is, an abstract data
> type and/or a set of functions or macros to manipulate fixed-point
> numbers, why did you do so?  If not, why not?

#### SURVEY QUESTIONS

Answering these questions is optional, but it will help us improve the
course in future quarters.  Feel free to tell us anything you
want--these questions are just to spur your thoughts.  You may also
choose to respond anonymously in the course evaluations at the end of
the quarter.

> In your opinion, was this assignment, or any one of the three problems
> in it, too easy or too hard?  Did it take too long or too little time?

> Did you find that working on a particular part of the assignment gave
> you greater insight into some aspect of OS design?

> Is there some particular fact or hint we should give students in
> future quarters to help them solve the problems?  Conversely, did you
> find any of our guidance to be misleading?

> Do you have any suggestions for the TAs to more effectively assist
> students, either for future quarters or the remaining projects?

> Any other comments?



