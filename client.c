/*
 *  client.c
 *
 *  Modified by Evan Graves on 3/3/13
 * 
 *  Client Program
 *  Send a string to a server using TCP
 *
 */


// Standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Socket-related libraries
#include <sys/types.h>
#include <sys/socket.h> 
#include <netdb.h>

#define MAX_LINE_SIZE 50

void print_usage(){
  fprintf(stderr, "Usage: ./client [hostname] [port]\n");
  fprintf(stderr, "Example: ./client 192.168.0.2 4845\n\n");
}
 
void check_args(int argc, char *argv[]){
  if( argc != 3 ) {
    print_usage();
	exit(1);
  }
}

int main(int argc, char *argv[])
{

  int success; //For error checking
  struct addrinfo hints;
  struct addrinfo *serverInfo;
  
  // Check arguments and exit if they are not well constructed
  check_args(argc, argv);
  
  // Fill the 'hints' structure
  memset( &hints, 0, sizeof(hints) );
  hints.ai_family   = AF_INET;     // Use IPv4 address
  hints.ai_socktype = SOCK_STREAM; // TCP sockets

  
  // Get the address information of the server.
  // argv[1] is the address, argv[2] is the port
  getaddrinfo(argv[1], argv[2], &hints, &serverInfo);
  
  // Create a socket to communicate with the server
  int socketFD = socket( serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol );
	//Error checking
	if (socketFD == -1) {perror("Failed to create socket"); exit(1);}
  

  // Connect to the server (only for TCP)
   success = connect(socketFD, serverInfo->ai_addr, serverInfo->ai_addrlen);
	//Error checking
	if (success == -1) {perror("Failed to connect to server"); exit(1);}

	//Instruct how to quit
  printf("Note: to quit client, type \"quit\" with no numbers\n");
  while(1){
  // Ask for data to send
  char line[MAX_LINE_SIZE];
  char text[MAX_LINE_SIZE];
  char quit[] = "quit";
  int encrypVal = -1;
  int arguments = 0;
  int len;
  while(arguments != 2)
   {
  	printf("Please input the string to send followed by an integer: ");
  	fgets(line, MAX_LINE_SIZE + 1, stdin);
  	len = strlen(line);
	arguments = sscanf(line,"%s %i",text,&encrypVal);
	//Check for quit string
	if ((strcmp (quit, text) == 0) && (encrypVal < 0)) {break;}
	if (arguments != 2) 
		{printf("Improper format. Example: california 4\n");}
	if (encrypVal > 99) //Check if exceeds max server can process
		{printf("Encryption value cannot exceed 99\n"); arguments = 1;}
   }

//Remove the unneccessary newline from the string
	int k;
	int end_index = 0; //Index of the space after string
	for (k = 0; k < MAX_LINE_SIZE; k++)
	{
		if(line[k] == ' ') {end_index = k;}
		if(line[k] == '\n')
			{line[k] = '\0'; break;}
	}


  //Check to see if quit was requested
  // -1 quits the client and its server thread
  // by sending 'Q' as the encryption value
  if(encrypVal < 0) {
	int i;
	for (i = 0; i < MAX_LINE_SIZE; i++)
	{
		if((line[i] == '\n') || (line[i] == '\0'))
			{line[i] = ' ';}
		if(line[i] == ' ')
		{ 
			line[i+1] = 'Q'; 
			line[i+2] = '\0';
			break;
		}
	}
  }
  
  // Send the data:
  printf("Sending string '%s' to '%s' using port '%s'... ", line, argv[1], argv[2]);

  success = send(socketFD, line, len, 0); // TCP style.
	//Error checking
	if (success == -1) {perror("Failed to send data"); exit(1);}

  printf("Done!\n\n");


//Exit while loop here if ready to quit
  if(encrypVal < 0) printf("Exiting\n");
  if(encrypVal < 0) break;


//RECEIVE DATA FROM SERVER HERE
	int num_bytes_received = 0;
  	char processed[MAX_LINE_SIZE];
	num_bytes_received = recv(socketFD, processed, MAX_LINE_SIZE-1, 0);
	if(num_bytes_received == -1) {perror("Failed to properly receive data"); exit(1);}

	//Clean up returned string by adding a null char at end
	processed[end_index] = '\0';
  printf("Your string encrypted using the key = '%i' is '%s'\n", encrypVal, processed);
    
} //End of while loop to accpet data


  // Free the address info struct and close the socket
  freeaddrinfo(serverInfo);
  success = close(socketFD);
	//Error checking
	if (success == -1) {perror("Failed to close socketFD"); exit(1);}
  
  return EXIT_SUCCESS;
}
