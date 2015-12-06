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

__Server__

    -bash-4.1$ ./server
    New client connection.
    ...............................................
    Current Bank Accounts:
    Empty bank.
    ...............................................

__Client_1__

    -bash-4.1$ ./client localhost
    Connected to Server.
    Client is prepared for commands.
    Server is prepared for commands.

    Server Message: Please enter a command:


## Testcase 2: Connecting Multiple Clients

__Server__

    -bash-4.1$ ./server
    New client connection.
    ...............................................
    Current Bank Accounts:
    Empty bank.
    ...............................................
    New client connection.

__Client_01__

    -bash-4.1$ ./client localhost
    Connected to Server.
    Server is prepared for commands.
    Client is prepared for commands.

    Server Message: Please enter a command:

__Client_02__

    -bash-4.1$ ./client localhost
    Connected to Server.
    Server is prepared for commands.
    Client is prepared for commands.

    Server Message: Please enter a command:


## Testcase 3: Functionality of Account Structures

__Server__

    -bash-4.1$ ./server
    New client connection.
    Server received command:    <open roshan>
    Server received command:    <open charles>
    Server received command:    <open goku>
    ...............................................
    Current Bank Accounts:
        Account Name: charles
        Balance: $0.00
        

        Account Name: goku
        Balance: $0.00
        

        Account Name: roshan
        Balance: $0.00
        

    ...............................................
    Server received command:    <open voldemort >
    Server received command:    <credit>
    ...............................................
    Current Bank Accounts:
        Account Name: charles
        Balance: $0.00
        

        Account Name: goku
        Balance: $0.00
        

        Account Name: voldemort
        Balance: $0.00
        

        Account Name: roshan
        Balance: $0.00
        

    ...............................................
    Server received command:    <debit>
    Server received command:    <balance>
    Server received command:    <finish>
    Server received command:    <start roshan>
    Server received command:    <balance>
    Server received command:    <finish>
    Ended session and unlocked account mutex
    Server received command:    <exit>
    ...............................................
    Current Bank Accounts:
        Account Name: charles
        Balance: $0.00
        
        
        Account Name: goku
        Balance: $0.00
        
        
        Account Name: voldemort
        Balance: $0.00
        
        
        Account Name: roshan
        Balance: $0.00
        
        
    ...............................................


__Client_01__

    -bash-4.1$ ./client localhost
    Connected to Server.
    Client is prepared for commands.
    Server is prepared for commands.
    
    Server Message: Please enter a command:
    open roshan
    Server Message: Successfully added account roshan.
    open charles
    Server Message: Successfully added account charles.
    open goku
    Server Message: Successfully added account goku.
    open voldemort 
    Server Message: Successfully added account voldemort.
    credit 10
    Server Message: Not currently in a session.
    debit 5
    Server Message: Not currently in a session.
    balance
    Server Message: Not currently in a session.
    finish
    Server Message: Not currently in a session.
    start roshan
    Server Message: Successfully connected to account roshan.
    balance
    Balance: $0.00
    finish
    Server Message: Finishing session to account roshan.
    exit



## Testcase 4: Testing Basic Commands on One Client

__Server__

    -bash-4.1$ ./server
    New client connection.
    ...............................................
    Current Bank Accounts:
    Empty bank.
    ...............................................
    Server received command:    <open bruce>
    ...............................................
    Current Bank Accounts:
        Account Name: bruce
        Balance: $0.00
        
        
    ...............................................
    Server received command:    <start bruce>
    Server received command:    <credit 10>
    Server received command:    <debit 5>
    Server received command:    <balance>
    Server received command:    <finish>
    Ended session and unlocked account mutex
    ...............................................
    Current Bank Accounts:
        Account Name: bruce
        Balance: $5.00
        
        
    ...............................................
    Server received command:    <exit>


__Client_01__

        -bash-4.1$ ./client localhost
        Connected to Server.
        Client is prepared for commands.
        Server is prepared for commands.

        Server Message: Please enter a command:
        open bruce
        Server Message: Successfully added account bruce.
        start bruce
        Server Message: Successfully connected to account bruce.
        credit 10
        Balance:    $0.00
        Credit: $10.00
        New Balance:    $10.00
        debit 5
        Old Balance:    $10.00
        Depositing: $5.00
        New Balance:    $5.00
        balance
        Balance: $5.00
        finish
        Server Message: Finishing session to account bruce.
        exit

        `````````````````````````````````````````````````````````


## Testcase 5: Serving Multiple Clients Simultaneously

__Server__

        -bash-4.1$ ./server
        New client connection.
        New client connection.
        Server received command:    <open obiwan>
        Server received command:    <open darthmaul>
        ...............................................
        Current Bank Accounts:
            Account Name: darthmaul
            Balance: $0.00
            

            Account Name: obiwan
            Balance: $0.00
            

        ...............................................
        Server received command:    <start darthmaul>
        Server received command:    <credit 1000>
        Server received command:    <start obiwan>
        Server received command:    <credit 50>
        Server received command:    <debit 3>
        ...............................................
        Current Bank Accounts:
            Account Name: darthmaul
            Balance: $1000.00
            IN SERVICE

            Account Name: obiwan
            Balance: $47.00
            IN SERVICE

        ...............................................
        Server received command:    <debit 4>
        Server received command:    <finish>
        Ended session and unlocked account mutex
        Server received command:    <finish>
        Ended session and unlocked account mutex
        Server received command:    <exit>
        Server received command:    <exit>


__Client_01__

        -bash-4.1$ ./client localhost
        Connected to Server.
        Server is prepared for commands.
        Client is prepared for commands.

        Server Message: Please enter a command:
        open obiwan
        Server Message: Successfully added account obiwan.
        start obiwan
        Server Message: Successfully connected to account obiwan.
        credit 50
        Balance:    $0.00
        Credit: $50.00
        New Balance:    $50.00
        debit 3
        Old Balance:    $50.00
        Depositing: $3.00
        New Balance:    $47.00
        finish
        Server Message: Finishing session to account obiwan.
        exit


__Client_02__

        -bash-4.1$ ./client localhost
        Connected to Server.
        Server is prepared for commands.
        Client is prepared for commands.

        Server Message: Please enter a command:
        open darthmaul
        Server Message: Successfully added account darthmaul.
        start darthmaul
        Server Message: Successfully connected to account darthmaul.
        credit 1000
        Balance:    $0.00
        Credit: $1000.00
        New Balance:    $1000.00
        debit 4
        Old Balance:    $1000.00
        Depositing: $4.00
        New Balance:    $996.00
        finish
        Server Message: Finishing session to account darthmaul.
        exit


## Testcase 6: Filling up Server to Limit

__Server__

        -bash-4.1$ ./server
        .
        .
        .


__Client_01__

        -bash-4.1$ ./client localhost
        Connected to Server.
        Client is prepared for commands.
        Server is prepared for commands.

        Server Message: Please enter a command:
        open 1
        Server Message: Successfully added account 1.
        open 2
        Server Message: Successfully added account 2.
        .
        .
        .
        Server Message: Successfully added account 19.
        open 20
        Server Message: Successfully added account 20.
        open 21
        Server Message: Cannot create more than twenty accounts.


## Testcase 7: Attempting to Open Duplicate Accounts

__Server__

        -bash-4.1$ ./server
        New client connection.
        Server received command:    <open sally>
        Server received command:    <open sally>
        Server received command:    <quit>

__Client_01__

        -bash-4.1$ ./client localhost
        Connected to Server.
        Server is prepared for commands.
        Client is prepared for commands.

        Server Message: Please enter a command:
        open sally
        Server Message: Successfully added account sally.
        open sally
        Server Message: Cannot create account sally because the account name already exists.




## Testcase 8: Starting Client Process Before Serve

__Server__

        -bash-4.1$ ./server
        New client connection.

__Client_01__

        -bash-4.1$ ./client localhost
        Attempting to connect to localhost...
        Connection refused
        Connected to Server.
        Client is prepared for commands.
        Server is prepared for commands.

        Server Message: Please enter a command:



## Testcase 9: Functionality of Credit and Debit

__Server__

        -bash-4.1$ ./server
        New client connection.
        Server received command:    <open 2pac>
        Server received command:    <start 2pac>
        Server received command:    <credit 1000>
        Server received command:    <debit 1000000>
        Server received command:    <debit 10>
        Server received command:    <balance>
        ...............................................
        Current Bank Accounts:
            Account Name: 2pac
            Balance: $990.00
            IN SERVICE

        ...............................................
        Server received command:    <finish>
        Ended session and unlocked account mutex
        Server Received Command:    <exit>


__Client_01__

        -bash-4.1$ ./client localhost
        Connected to Server.
        Client is prepared for commands.
        Server is prepared for commands.

        Server Message: Please enter a command:
        open 2pac
        Server Message: Successfully added account 2pac.
        start 2pac
        Server Message: Successfully connected to account 2pac.
        credit 1000
        Balance:    $0.00
        Credit: $1000.00
        New Balance:    $1000.00
        debit 1000000
        Server Message: Not enough funds to debit this amount.
        debit 10
        Old Balance:    $1000.00
        Depositing: $10.00
        New Balance:    $990.00
        balance
        Balance: $990.00
        finish
        Server Message: Finishing session to account 2pac.
        exit


## Testcase 10: Attempting to Start on n Active Session

__Server__

        -bash-4.1$ ./server
        New client connection.
        Server received command:    <open steve>
        Server received command:    <open jobs>
        New client connection.
        Server received command:    <start jobs>
        Server received command:    <start jobs>
        Error attempting to lock already locked mutex: 16
        ...............................................
        Current Bank Accounts:
            Account Name: steve
            Balance: $0.00
            

            Account Name: jobs
            Balance: $0.00
            IN SERVICE

        ...............................................
        Error attempting to lock already locked mutex: 16
        Error attempting to lock already locked mutex: 16
        Error attempting to lock already locked mutex: 16
        Server received command:    <finish>
        Ended session and unlocked account mutex
        Server received command:    <credit 10>
        Server received command:    <finish>
        Ended session and unlocked account mutex
        ...............................................
        Current Bank Accounts:
            Account Name: steve
            Balance: $0.00
            

            Account Name: jobs
            Balance: $10.00
            

        ...............................................
        Server received command:    <exit>
        Server received command:    <exit>
    

__Client_01__

        -bash-4.1$ ./client localhost
        Connected to Server.
        Client is prepared for commands.
        Server is prepared for commands.

        Server Message: Please enter a command:
        open steve
        Server Message: Successfully added account steve.
        open jobs
        Server Message: Successfully added account jobs.
        start jobs
        Server Message: Account jobs is already in use.
        Server Message: Waiting to start session for account jobs.
        Server Message: Waiting to start session for account jobs.
        Server Message: Waiting to start session for account jobs.
        Server Message: Waiting to start session for account jobs.
        Server Message: Successfully connected to account jobs.
        credit 10
        Balance:    $0.00
        Credit: $10.00
        New Balance:    $10.00
        finish
        Server Message: Finishing session to account jobs.
        exit



__Client_02__

        -bash-4.1$ ./client localhost
        Connected to Server.
        Client is prepared for commands.
        Server is prepared for commands.

        Server Message: Please enter a command:
        start jobs
        Server Message: Successfully connected to account jobs.
        finish
        Server Message: Finishing session to account jobs.
        exit


## Testcase 11: Client Exits with Serve Exit

__Server__

        -bash-4.1$ ./server
        New client connection.
        ^C
        -bash-4.1$ 


__Client_01__

        -bash-4.1$ ./client localhost
        Connected to Server.
        Client is prepared for commands.
        Server is prepared for commands.

        Server Message: Please enter a command:
        -bash-4.1$ 
