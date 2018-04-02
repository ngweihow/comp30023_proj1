/**
 * Project Code for Computer Systems COMP30023 2018 S1
 * A simple HTML 1.0 Web Server which handles basic GET requests 
 * To compile: gcc
 * Written by: 828472
 */

// Library and header includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

// Constant Declarations and Initialisation
#define BUFFERSIZE 256


// Function Declarations 
char* concat(char* s1, char* s2);


//--------------------------------------------------------------------------------------------------
/* Main Functions
 *
 */

// Main Function
int 
main(int argc, char *argv[])
{   
    // Declaring the sockets file descriptions and the port number.
    int sockfd, newsockfd, portno;// clilen;
    // Char array used as a string buffer.
    char buffer[256];
    // Declaring the string later used to assign to the root directory.
    char* root;

    // Structs for server and client address
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;


    // Checking if the number of command line arguements are valid
    if (argc < 3) {
        fprintf(stderr,"ERROR, no port or root path provided! \n");
        exit(1);
    }


    // Saving the Port Number as an int
    portno = atoi(argv[1]);
    // Copy and then concatenating the root path with the one from the request


    // ---------------------------------------
    /* Creating TCP Socket*/

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Return an error if the socket cannot be opened
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    
    
    // ---------------------------------------

    return 0;
}


//--------------------------------------------------------------------------------------------------
/* Helper Functions
 *
 */

// Concatenate function to appending one string to another
char*
concat(char* s1, char* s2) {   
    // Determine the length of the output and allocating memory for it
    int output_len = strlen(s1) + strlen(s2) + 1;
    char* concat_str = (char*)malloc(output_len * sizeof(char));

    // Copy the first string into the allocated return string
    strcpy(concat_str, s1);
    // Concat the second on onto the allocated return string afterwards
    strcat(concat_str, s2);

    return concat_str;
}





