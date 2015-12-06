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

# Data Structures & Helper Files

My program implementation uses a Tokenizer structure to tokenize and parse through inputs. This is a slightly altered version of `pa1`. Additionally, I have implemented a hashmap to store and request account structures within the server program. This made dealing with data within the server program very simple, easy, and efficient.

![image1](/assets/images/image1.png)

The data structure used to store account information is a structure named account. Its declaration is shown above. It contains 4 fields: a mutex type, a string, a double, and a boolean. Each of these are self-explanatory. 

An account structure is created upon the `open` command and can be accessed by a client with the `start` command. 

Starting an account will activate it, changing the boolean field to reflect this state. When a client is within an account, they have permissions over  changing this account’s balance. 

Each client program accessing the banking  server is assigned a `clientProcess` data  structure. Its declaration is shown below the account declaration. This structure stores a file descriptor of the connection, `FD_connect`, as well as additional information about which account it is  currently accessing with `start`. This will help ensure that a client cannot access more than one account at a time.
