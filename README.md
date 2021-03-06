# 3_rd-sem
![GitHub repo size](https://img.shields.io/github/repo-size/derzhavin3016/3_rd-sem?style=for-the-badge)
![GitHub last commit](https://img.shields.io/github/last-commit/derzhavin3016/3_rd-sem?color=red&style=for-the-badge)
![GitHub lines](https://img.shields.io/tokei/lines/github/derzhavin3016/3_rd-sem?style=for-the-badge)

My homework from 3-rd semester in MIPT.

## lsn1

* ```mye.c``` - my own soft version of ```echo``` utility. The key ```-n``` is realised.
* ```hello.c``` - print "Hello world" to console by using ```write()``` syscall.

## lsn2

* ```myid.c``` - my own soft version of ``` id ``` utility. Realised searching by uid and username.

## lsn3

* ```mycat.c``` - my own soft version of ```cat``` utility. Realised writing from stdout or from some files. 
* ```mycp.c``` - my own soft version of ```cp``` utility. Realised keys ```-f```, ```-v```, ```-i``` and long analogs. Works in 2 modes:
  ```
  mycp [OPTIONS] source_file destination_file
  ```
  and
  ```
  mycp [OPTIONS] file_1 ... file_N destination_folder
  ```

## lsn4

* ```myfork.c``` - program which creates N streams in program and print it's ```PID``` and ```PPID``` in terminal.
* ```sleepsort.c``` - sorting numbers by sleeping in parallel streams (time depends of number) and printing numbers.
* ```mytime.c``` - utility which measure executing tume of any program by using ```exec``` functions family.
* ```myls.c``` - my own soft version of ```ls``` utility. (in process)

## lsn5

* ```myexecwc.c``` - version of ```mytime.c``` with key ```--wc``` which count strings, words and bytes from **STDOUT** of executing program. Pipe is used.
* ```myshell.c``` - soft shell environment program. Wait for input in form 
  ```
  $ prog_1 [OPTIONS] | ... | prog_N [OPTIONS]
  ```
  and organize ```n - 1``` succesive pipes in described order. (in process, not ready yet)

## lsn6

* ```runners.c``` - demonstration program, which simulates a runners competition. Creates a judge and ```N``` runners (from cmd args), and then inform about every action : 
  * The judge and runners come to the competition and print a message about it.
  * Judge waits for all runners.
  * Judge start the competition, and give stick to the first runner.
  * First runner gives stick to second and so on. 
  * Judge end the competition and all runners go home.

  All interaction organized by messages ```SCv5```.

## lsn7

* ```chill.c``` - demonstration program, which simulates a ship & passengers models. Creates a capitan and ```n``` passengers (from cmd args).

## lsn8

* ```client.c``` and ```server.c``` - demonstartion program, which simulates an interaction between client and server:
  * Client work similary to ```cat``` utility he get lines from **stdin** or from file(s) and then write it to **shared memory** line by line.
  * Server waits while any client writes to **shared memory** and then print line in shared memory to **stdout**.
  An interacction organized by  semaphores in critical section. There are two binary semaphores, actually:
  * MEM - semaphore which shows state of memory - 1 memory has changed, 0 memory has not changed.
  * CHAN - semaphores which shows state of communication channel between client and server, 1 - channel is free, 0 - channel is not free.

## lsn9

* ```thr.c``` - working with threads program, which increment a variable up to ```n``` value by using  ```m``` **posix threads**. It has 4 implementations:
  1. Native - every thread from ```m``` threads increments variable ```n / m``` times. It works really bad - every time a different value as a result.
  2. Critical section for increment - every time a right value, but works too slow.
  3. Critical section for incremention cycle - also slow(
  4. Every thread has its own local variable to increment, and then he just add it to global varibale using critical section - works rather faster than 1 algorithm!!!

  **Note**: Compile with ```-pthread``` key (described in Makefile)
  
  **Program usage**:
  ```bash
  ./th expected_value number_of_threads number_of_algorithm(described upper)
  ``` 

## lsnA
## lsnB

* ```shm.c``` - demo program which creates two processes - a client and a server, the communication organized with **POSIX**:
  * ***shared memory*** - for sending data.
  * ***message queue*** - for synhronization.
  
  **Usage**:
  ```bash
  ./shm INPUT_FILE OUTPUT_FILE
  ```
  Client reads from **INPUT_FILE** and write the data to shared memory,
  then Server reads data from shared memory and put it to **OUTPUT_FILE**.
  For convenience, the shared memory file mapped into virtual mempry by using ```mmap()```
  function.
  
  **Note**:
  Link with ```-lrt``` key (described in _CMakeLists.txt_)
