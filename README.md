# multithread-bank-system

    Roshan Patel
    198:211 SYSTEMS PROGRAMMING
    PA5: MUTLITHREADED-BANK-SERVER–readme.pdf

# Overview of Program Use

My implementation of the multithreaded bank server can be compiled using the provided makefile. It will produce two executables: server and client. This project includes several utilities which are compiled into object files to produce the exectuables. These include tokenizers and hashmap implementations. The following shows the proper usage of my program. 

__NOTE:__ The server port is automatically set to `60221`. This is arbitrarily chosen to be the same as Avogadro’s number.The port can be changed by altering the `port.h` file. 

```sh
make
./server
#  Open a new terminal tab 
./client localhost
```

