# 3_rd-sem
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

* ```runners.c``` - demonstration program, which simulates a runners competition. Creates a judge and ```N``` runners (from cmd args), and then inform about every action : firstly, the judge and runners come to the competition and print a message about it, then judge waits for all runners and then start the competition, and give command to start to the first runner, then first runner gives command to second and so on. After all, judge end the competition and all runners go home.
