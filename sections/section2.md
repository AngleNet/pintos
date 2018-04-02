* Hello world
    ```
    Case1:
        Hello World: 90210
        Hello World: 0
    Case2:
        Hello World: 0
        Hello World: 90210
    ```
* Forks
    ```
    P1-->S1, S2, S1-->G1
    P2-->S3
    P3
    
    7
    ```
* Stack allocation
    ```
    Stuff is 7
    Stuff is 7
    ```
* Heap allocation
    ```
    Stuff is 7
    Stuff is 7
    ```

* Slightly more complex heap allocation
    ```

    ```

* Simple wait
    ```
    Hello World
    : 0
    Hello World
    : 90210 

    int main(void){
        pid_t pid = fork();
        int exit;
        if(pid != 0){
            waitpid(pid, &exit, 0);
        }
        printf("Hello World\n: %d\n", pid);
        return 0;
    }
    ```

3.6 Fork and file descriptors

3.7 Exec

3.8 Exec + fork

3.9 Implementing fork() efficiently
