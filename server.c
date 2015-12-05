#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include "tokenizer.h"
#include "linkedlist.h"
#include "hashmap.h"
#include "port.h"
#include <pthread.h>

#define _POSIX_C_SOURCE 200112L



/* Account structure */
struct account {
	pthread_mutex_t mutex;
	string name;
	double balance;
	bool active;
};
typedef struct account* account;

/* Client connection structure */
struct clientProcess {
	int FD_connect;
	pthread_t thread;
	account acc;
};
typedef struct clientProcess* clientProcess;

/* Memory unit structure for void* data handling */
struct memUnit {
	string name;
	void* data;
};
typedef struct memUnit* memUnit;


/* GLOBAL VARIABLES */
map accounts;			// Hashmap of accounts
linkedList clients;		// LinkedList of client connections
pthread_mutex_t accounts_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;


/* 
	Prints out account information upon recieving a signal.
	@param signum 	Signal from timer
*/
void printStatus(int signum)
{
	int flag = 0;
	if(pthread_mutex_trylock(&accounts_mutex) == 0){
		printf("...............................................\n");
		printf("Current Bank Accounts:\n");
		for(int n = 0; n < 20; n++){
			if(accounts->values[n]){
				flag = 1;
				linkedList l = accounts->values[n];
				Node n = l->head;

				while(n){
					account acc = (account) ((memUnit) n->data)->data;
					string name = acc->name;
					double balance = acc->balance;
					int active = acc->active;
					printf("\tAccount Name: %s\n\tBalance: $%.2f\n\t%s\n\n", name, balance, active ? "IN SERVICE" : "");
					n = n->next;
				}
			}
		}

		if (!flag) printf("Empty bank.\n");
		printf("...............................................\n");

		pthread_mutex_unlock(&accounts_mutex);
	}
	else{
		printf("unable to acquire mutex lock when printing stats\n");	
	}
}


/*
	End a connection with a client program.
	@param client_process 	Pointer to clientProcess to cease connections with	
*/
void exitClient(void* client_process)
{
	if(client_process){
		clientProcess c = (clientProcess) client_process;
		free(c);
	}
}


/* 
	Create a new account for the bank.
	@param string 	Name of new account
	@return 		Newly created account of type account
*/
account openAccount(string token) {
	account acc = (account) malloc (sizeof(struct account));
	acc->name = token;
	acc->active = false;
	acc->balance = 0;
	pthread_mutexattr_t attr;

	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

	if(pthread_mutex_init(&acc->mutex, &attr)){
		perror("Error could not create account mutex.");
		exit(0);
	}

	return acc;
}


int streq(char* a, char* b)
{
	return strcmp(a, b) == 0;
}


/*
	This function handles commands from connected client programs.
	Commands that are available: open, start, credit, debit, balance, finish, exit
	@param c 	Client process being served
	@return 	NULL
*/
void* clientHandler(void* c)
{	
	clientProcess client = (clientProcess) c;
	int socketDescriptor = client->FD_connect;
	pthread_detach(pthread_self());
	double amt = 0;
	char request[2048];
	Tokenizer tokenizer = 0;

	write(socketDescriptor, request, sprintf(request, "\nServer Message:\tPlease enter a command:\n") + 1);

	while(read(socketDescriptor, request, sizeof(request)) > 0){
		if(streq(request, "0") || streq(request, ""))
			continue;

		printf("Server received command:	<%s>\n", request);

		tokenizer = createTokenizer(request);
		string token = 0;

		token = getNextToken(tokenizer);

		if (streq(token, "open")) {
			/* Invalid syntax */
			if (!(token = getNextToken(tokenizer)) || getNextToken(tokenizer)) {
				write(socketDescriptor, request, sprintf(request, "Server Message: Invalid syntax.\n") + 1);
			}
			/* Exceeded max account limit */
			else if (accounts->count == 20) {
				write(socketDescriptor, request, sprintf(request, "Server Message: Cannot create more than twenty accounts.\n") + 1);
			}
			/* Duplicate account name */
			else if (getKeyValue(token, accounts)) {
				write(socketDescriptor, request, sprintf(request, "Server Message: Cannot create account %s because the account name already exists.\n", token) + 1);
			}
			/* Creates an account with an input string and add to hashmap. */
			else {
				account acc = openAccount(token);
				pthread_mutex_lock(&accounts_mutex);
				insertMapValue(acc->name, acc, accounts);
				pthread_mutex_unlock(&accounts_mutex);
				write(socketDescriptor, request, sprintf(request, "Server Message: Successfully added account %s.\n", acc->name) + 1);
			}
		}
		else if (streq(token, "start")) {
			/* Invalid syntax */
			if (!(token = getNextToken(tokenizer)) || getNextToken(tokenizer)) {
				write(socketDescriptor, request, sprintf(request, "Server Message: Invalid syntax.\n") + 1);
				continue;
			}
			/* Already in session with this account */
			if (client->acc) {
				write(socketDescriptor, request, sprintf(request, "Server Message: Already serving account with account name %s.\n", client->acc->name) + 1);
				continue;
			}
			/* Account name does not exist in hashmap */
			if ((client->acc = (account) getKeyValue(token, accounts)) == NULL) {
				write(socketDescriptor, request, sprintf(request, "Server Message: Account with name %s does not exist.\n", token) + 1);
				continue;
			} 
			/* Connect to account */
			account acc = client->acc;
			if (client->acc->active) {
				write(socketDescriptor, request, sprintf(request, "Server Message: Account %s is already in use.\n", token) + 1);
				client->acc = NULL;
			} 
			while(pthread_mutex_trylock(&acc->mutex) != 0){
				printf("Error attempting to lock already locked mutex: %d\n", pthread_mutex_trylock(&acc->mutex));
				write(socketDescriptor, request, sprintf(request, "Server Message: Waiting to start session for account %s.\n", token) + 1);
				sleep(2);
			}
			client->acc = acc;
			client->acc->active = true;

			write(socketDescriptor, request, sprintf(request, "Server Message: Successfully connected to account %s.\n", token) + 1);
		} 
		else if (streq(token, "credit")) {
			/* Client program is not within a session. */
			if (!client->acc || !client->acc->active) {
				write(socketDescriptor, request, sprintf(request, "Server Message: Not currently in a session.\n") + 1);
			}
			/* Invalid syntax */
			else if (!(token = getNextToken(tokenizer)) || getNextToken(tokenizer)) 			{
				write(socketDescriptor, request, sprintf(request, "Server Message: Invalid syntax.\n") + 1);
			}
			/* Credit number is invalid */
			else if (!(amt = atof(token)) || amt <= 0) {
				write(socketDescriptor, request, sprintf(request, "Server Message: Not a valid credit amount.\n") + 1);
			}
			/* Add credit amount to account in session */ 
			else {
				write(socketDescriptor, request, sprintf(request, "Balance:\t$%.2lf\nCredit:\t$%.2lf\nNew Balance:\t$%.2lf\n", client->acc->balance, amt, (client->acc->balance + amt)) + 1);
				client->acc->balance += amt;
			}
		} 
		else if (streq(token, "debit")) {
			/* Client program is not within a session */
			if (!client->acc || !client->acc->active) {
				write(socketDescriptor, request, sprintf(request, "Server Message: Not currently in a session.\n") + 1);
			}
			/* Invalid syntax */
			else if (!(token = getNextToken(tokenizer)) || getNextToken(tokenizer)) {
				write(socketDescriptor, request, sprintf(request, "Server Message: Invalid syntax.\n") + 1);
			}
			/* Debit amount is invalid */
			else if (!(amt = atof(token)) || amt <= 0) {
				write(socketDescriptor, request, sprintf(request, "Server Message: Not a valid debit amount.\n") + 1);
			}
			/* Debit amount exceeds funds */
			else if (amt > client->acc->balance) {
				write(socketDescriptor, request, sprintf(request, "Server Message: Not enough funds to debit this amount.\n") + 1);
			}
			/* Debit amount from current balance */
			else {
				write(socketDescriptor, request, sprintf(request, "Old Balance:\t$%.2lf\nDepositing:\t$%.2lf\nNew Balance:\t$%.2lf\n", client->acc->balance, amt, (client->acc->balance - amt)) + 1);
				client->acc->balance -= amt;
			}
		} 
		else if (streq(token, "balance")) {
			/* Client program is not within a session */
			if (!client->acc || !client->acc->active) {
				write(socketDescriptor, request, sprintf(request, "Server Message: Not currently in a session.\n") + 1);
			}
			/* Invalid syntax */
			else if (getNextToken(tokenizer)) {
				write(socketDescriptor, request, sprintf(request, "Server Message: Invalid syntax.\n") + 1);
			}
			/* Print current balance */
			else {
				write(socketDescriptor, request, sprintf(request, "Balance: $%.2lf\n", client->acc->balance) + 1);
			}
		} 
		else if (streq(token, "finish")) {
			/* Client program is not within a session */
			if (!client->acc || !client->acc->active) {
				write(socketDescriptor, request, sprintf(request, "Server Message: Not currently in a session.\n") + 1);
			} 
			/* Finish the session. */
			else {
				write(socketDescriptor, request, sprintf(request, "Server Message: Finishing session to account %s.\n", client->acc->name) + 1);
				client->acc->active = false;
				pthread_mutex_unlock(&client->acc->mutex);
				client->acc = NULL;
			
				printf("Ended session and unlocked account mutex\n");
			}
		} 
		else if (streq(token, "exit")) {
			write(socketDescriptor, request, sprintf(request, "Server Message: Disconnecting from server. Terminating client thread.\n") + 1);
			if (client->acc){
				client->acc->active = false;
				pthread_mutex_unlock(&client->acc->mutex);
			}
			pthread_mutex_lock(&clients_mutex);
			removeLinkedNode(client, clients);
			pthread_mutex_unlock(&clients_mutex);
			exitClient(client);
			close(socketDescriptor);

			destroyTokenizer(tokenizer);
			break;
		} 
		else {
			write(socketDescriptor, request, sprintf(request, "Invalid syntax. Available commands:\nopen <account_name> \topens a new account\nstart <account_name>\tstarts a customer session for an account\ncredit <amount>\t\tadd amount into account\ndebit <amount>\t\tremove amount from account\nbalance\t\t\treturns account balance\nfinish\t\t\tfinishes customer session\nexit\t\t\tdisconnects from server\n") + 1);
		}

		destroyTokenizer(tokenizer);
	}

	return NULL;
}

/*
	Helper function to start timer for printing bank account contents
*/
void beginTimer(int seconds)
{	
	struct itimerval timer;
	timer.it_value.tv_sec = seconds;
	timer.it_value.tv_usec = 0;
	timer.it_interval.tv_sec = seconds;
	timer.it_interval.tv_usec = 0;
	setitimer (ITIMER_REAL, &timer, NULL);
}

int main()
{
	/* File descriptor for listening to port */
	int FD_listen = socket(AF_INET, SOCK_STREAM, 0);
	int FD_connect = 0;
	struct sockaddr_in serverAddress;

	/* Hashmap of accounts with max limit of 20 */
	accounts = newMap(20, 1);
	/* LinkedList of client connections */
	clients = newLinkedList(NULL);

	memset(&serverAddress, '0', sizeof(serverAddress));

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

	/* Set up timer with signal handlers */
	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = printStatus;
	sigaction (SIGALRM, &sa, NULL);

	beginTimer(20);

 	/* Port can be changed in port.h */
	serverAddress.sin_port = htons(SERVER_PORT);

	if(bind(FD_listen, (struct sockaddr*) &serverAddress, sizeof(serverAddress))){
		perror("Error binding to port.");
		return 0;
	}

	if(listen(FD_listen, 100)){
		perror("Error listening to port.");
		return 0;
	}

	while(1){
		FD_connect = accept(FD_listen, NULL, NULL);
		if(FD_connect == -1){
			continue;
		}

		printf("New client connection.\n");
		/* Add new client connection to linkedList of connections */
		clientProcess client = malloc(sizeof(*client));
		client->FD_connect = FD_connect;
		client->acc = 0;
		Node node = newLinkedNode(client);

		/* Mutex lock for adding new clients to the linked list structure */
		pthread_mutex_lock(&clients_mutex);
		appendLinkedNode(node, clients);
		pthread_mutex_unlock(&clients_mutex);

		/* Create a new thread for this client connection */
		pthread_create(&client->thread, NULL, clientHandler, client);

		/* Throttle delay */
		sleep(1);
	}

	pthread_mutex_lock(&clients_mutex);
	deleteLinkedList(clients, exitClient);
	pthread_mutex_unlock(&clients_mutex);
	close(FD_listen);
	close(FD_connect);

	return 0;
}
