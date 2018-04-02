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

// Function Declarations 
char* concat(char* s1, char* s2);


//--------------------------------------------------------------------------------------------------

// Main Function
int 
main(int argc, char const *argv[])
{   
    // Declaring the sockets file descriptions and the port number.
    int sockfd, newsockfd, portno;// clilen;
    // Char array used as a string buffer.
    char buffer[256];
    // Declaring the string later used to assign to the root directory.
    char* root;


    // Checking if the number of command line arguements are valid
    if (argc < 3) 
    {
        fprintf(stderr,"ERROR, no port or root path provided! \n");
        exit(1);
    }


    // Saving the Port Number as an int
    portno = atoi(argv[1]);
    // Copy and then concatenating the root path with the one from the request

    return 0;
}


//--------------------------------------------------------------------------------------------------

// Concatenate function to appending one string to another
char*
concat(char* s1, char* s2) 
{   
    // Defining a buffer string variable for strcpy and the final return string
    char* buffer[256];
    char* concat_str = "\0";

    // Copy the first string into the buffer
    strcpy(buffer, s1);
    // Concat the second on onto the buffer afterwards
    strcat(buffer, s2);

    return
}


