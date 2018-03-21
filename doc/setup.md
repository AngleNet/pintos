### Installing
* Prerequisites
    > gcc, gnu binutils(addr2line, ar, ld, objcopy, and ranlib) <br>
    > perl, gnu make, qemu >= 0.8.0, gdb, Texinfo, TeX
* Installation
    * Add pintos `src/utils` to PATH
    * Install gdb-macros to pintos-gdb
    * `cd src/utils && make`
* Test debug

    Create a `tmux` window
        
        tmux new -s pintos # create a new session
        ctrl-b % # split virtically
        ctrl-b " # split horizontally
        ctrl-b c # create window
        ctrl-b NUM # switch window
        ctrl-b s # session list
        ctrl-b d # detach a session
        tmux attach # attach a session
        ctrl-b [ # copy mode


    Run `cd threads && make && cd build && pintos --gdb -- run
    alarm-multiple` to compile threads module and start pintos 
    in **debug** mode. Run `cd threads/build && pintos-gdb kernel.o`,
    Now we in the gdb.

        debugpintos
        # Need to break at a specified location. step or next will not 
        # work here.
        b main # Start debuging here

        # Redirect sub window to other termimals
        dashboard source -output /dev/pts/14
        dashboard assembly -output /dev/pts/21
        dashboard register -output /dev/pts/20 
        dashboard stack  -output /dev/pts/19
        dashboard threads  -output /dev/pts/18
        dashboard memory  -output /dev/pts/17
        # write the above redirectings to gdb-macros

    The `tmux` terminal should like this:
    ![pintos](../img/pintos-gdb-tmux.png)

