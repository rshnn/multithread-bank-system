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


# Thread Syncronization

My program implementation uses a Tokenizer structure to tokenize and parse through inputs. This is a slightly altered version of pa1. Additionally, I have implemented a hashmap to store  and request account structures within the server program. This made dealing with data within  the server program very simple, easy, and efficient.

![image2](/assets/images/image2.png)

Thread synchronization was meticulously handled between the server and client programs. The server program starts by initiating a file descriptor to listen to the port number declared in `port.h`. Timers are set to print the contents of the bank account ever 20 seconds. This operates based on a signal handler, `SIGALRM`. The sever process is terminated with a `SIGINT`. 

When a connection is heard from the port, a `clientProcess` structure is created for it as well as its own thread. Mutex locks are utilized to ensure that no other processes may enter its thread. The server then accepts commands from the client program and alters the data structures accordingly. Responses are sent to the client program in reaction to commands received. 

As per the guidance of the assignment directions, the client produces two threads. The first thread is dedicated to command-inputting. This will read commands from the user and send them to the server. This is the second thread declared and it is handled by the `sendCommand` function. 

The second thread is a response-output thread. It will read messages from the server and forward them to the client program user. This is the first thread initialized (handled by the handler function). If clients cannot connect to the sever, it will retry every 3 seconds. The `sendCommand()` function will sleep for 2 seconds after accepting a command to throttle client command entry. Client processes will terminate with a `SIGINT` or the `exit` command. 

The client will announce its completion of connecting to the sever with a series of print statements indicating when the threads have all been initialized. The sever program announces its acceptance of a connection from a client with the print statement "New client connection." When clients disconnect from the server, messages are printed in the server program stating: “Ended session and unlocked account mutex.” The server will display print messages upon receiving any command from any connected client.

Lastly, the client will display messages sent  from the sever upon successful or failed commands.


# Testcase Analysis

Test cases attempt to tackle specific functions of the banking system systematically. First, I tested the proper connection between server and client processes. Then, I tested the proper sharing of data between the two processes. Lastly, I tested the proper implementation of banking functions. More information regarding my test cases and expected outputs can be found in the attached text files: `bank-testcases.txt` and `testplan.txt`. 

## Testcase 1: Testing Connections
