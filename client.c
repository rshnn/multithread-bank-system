#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include "port.h"

/* GLOBAL VARIABLES */
int 		FD_listen;
int 		to_exit;
pthread_t 	thread;
pthread_t 	inHandler;

/*
	String comparator shorthand function for determining
	equivalence of two strings with logical correctness.
	@param 		input string 1
	@param 		input string 2
	@return 	bool value of equivalence
*/
int streq(char* a, char* b){
	return strcmp(a, b) == 0;
}


/*
	This function will accept input commands from the 
	user and forward them to the server if they are valid.
	If an exit command is received, the client program 
	will terminate.
	Sleeps for 2 seconds between command inputs for command
	throttling between multiple client sessions.
*/
void* sendCommand(void* ignore)
{
	printf("Client is prepared for commands.\n");
	char input[2048];
	while(1){
		scanf(" %[^\n]", input);
		write(FD_listen, input, strlen(input) + 1);
		/* Client program is terminated with a SIGINT */
		if(streq(input, "exit")){
			pthread_kill(thread, SIGINT);
			printf("Successfully disconnected from server.\n\tHave a nice day.\n");
			exit(0);
		}
		/* Two second delay between user commands */
		sleep(2);
	}
}

/*
	Signal handler function.
	Recieves output signal from server program and
	responds with a print statement.
*/
void* handler(void* serverName)
{
	char* server = (char*) serverName;
	char request[2048];
	printf("Server is prepared for commands.\n");
	while(read(FD_listen, request, sizeof(request)) > 0){
		printf("%s", request);
	}

	return NULL;
}


/* 
	This function will locate and connect to the server program.
	Note:  Hints provided during lecture.
 */
void set_iaddr_str( struct sockaddr_in * sockaddr, char * x, unsigned int port )
{
	struct hostent * hostptr;

	memset( sockaddr, 0, sizeof(sockaddr) );
	sockaddr->sin_family = AF_INET;
	sockaddr->sin_port = htons( port );
	if ( (hostptr = gethostbyname( x )) == NULL ) {
		printf( "Error getting addr information\n" );
	}
	else {
		bcopy( hostptr->h_addr_list[0], (char *)&sockaddr->sin_addr, hostptr->h_length );
	}
}

int main(int argc, char** argv)
{
	if(argc != 2){
		printf("Error: Server host must be specified as a command line argument.");
		return 0;
	}

	int x = 0;
	char* serverName = argv[1];

	FD_listen = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serverAddress;

	set_iaddr_str(&serverAddress, serverName, SERVER_PORT);

	while( (x = connect(FD_listen, (const struct sockaddr*) &serverAddress, sizeof(serverAddress)) ) ) {
		printf("Attempting to connect to %s...\n", serverName);
		perror("");
		sleep(3);
	}
 
	/* Connection with server established. */
	printf("Connected to Server.\n");

	pthread_create(&thread, NULL, handler, serverName);
	pthread_detach(thread);

	/* Receive user commands. */
	pthread_create(&inHandler, NULL, sendCommand, NULL);
	pthread_detach(thread);

	while(write(FD_listen, "0", 2) == 2){
		sleep(1);
	}

	/* Garbage collection and safe exiting on SIGINT. */
	pthread_kill(thread, SIGINT);
	pthread_kill(inHandler, SIGINT);
	close(FD_listen);
	printf("Disconnected from server.\n");

	return 0;
}
