/**
 * Project Code for Computer Systems COMP30023 2018 S1
 * A simple HTML 1.0 Web Server which handles basic GET requests 
 * To compile: Run the Makefile with "make" OR
 *             gcc -o server_real server_real.c
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
#define BUFFERSIZE 8192
#define SPACE " "

// Resquest Reponses
char* const res200 = "HTTP/1.0 200 OK\r\nContent-Type: %s\r\n\r\n";
char* const res404 = "HTTP/1.0 404 NOT FOUND\r\nContent-Type: %s\r\n\r\n";


// Function Declarations 
char* concat(char* s1, char* s2);
void print_res(int sockfd, int response, int bytes);
int read_file(int sockfd, char* buffer);
void copy_to_buffer(FILE *fp, int *n, char* buffer);
char* parse_header(const char *str, const char *space);

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
    char* buffer;

    // Array used for header buffer
    char* header_buffer[BUFFERSIZE]

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

    // Allocating memory for server_addr
    memset(&serv_addr, '0', sizeof(serv_addr));


    /* Create address we're going to listen on (given port number)
     - converted to network byte order & any IP address for 
     this machine */
    
    // Determining what type of socket is created using socket()
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    // Specify the port to use for connections
    serv_addr.sin_port = htons(portno);


    // ---------------------------------------
    /* Binding*/

    // Binding socket to the specified address
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    // ---------------------------------------
    /* Listening*/

    /* Listen on socket - means we're ready to accept connections - 
     incoming connection requests will be queued */
    listen(sockfd,10);
    
    clilen = sizeof(cli_addr);

    // ---------------------------------------
    /* Accepting*/

    /* Accept a connection - block until a connection is ready to
     be accepted. Get back a new file descriptor to communicate on. */

    // Loop forever if there are requests coming from the socket
    while(1) {
        // Assigning a new socket to the accepted one
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        // Exit on error with accept
        if (newsockfd < 0) {
            perror("ERROR on accept");
            exit(1);
        }    

        // Allocate Space to the buffer for the header
        memset(&header_buffer, '0', BUFFERSIZE-1);

        // Read characters from the socket and then process them
        int n = read_file(newsockfd, header_buffer);

        // Error Handling if error from reading to socket
        if(n < 0) {
        perror("ERROR reading from socket");
        exit(1);
        }


        // Parse the header in the buffer and extract the path
        char* relative_path = parse_header(header_buffer, SPACE);
        // Free the header char buffer used
        free(header_buffer);
        

        // Concatenate the relative_path with the given root
        char* abs_path = concat(argv[2], relative_path);
        free(relative_path);

        // Free abs_path somewhere
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
    char* concat_str = malloc(output_len * sizeof(char));

    // Copy the first string into the allocated return string
    strcpy(concat_str, s1);
    // Concat the second on onto the allocated return string afterwards
    strcat(concat_str, s2);

    return concat_str;
}

// Print function to output the response that the server would give
void 
print_res(int sockfd, int response, int bytes) {
    int n;
    // Check the type of response coming in

    switch(response) {
        case 200:
        write(sockfd,res200,bytes);
        break;

        case 404:
        write(sockfd,res404,bytes);
        break;
    }

    // Break the program if there are errors writing
    if (n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }
}

/* File Reading function to parse the GET request
 * ----------------------------------------------
 * sockfd: The socket in which the file is being read from
 * buffer: The string/array where the binary file is being read onto
 *
 * returns: The number of bytes the file contains (that was read into buffer)
 */
int
read_file(int sockfd, char* buffer) {
    FILE *fp;
    unsigned long file_len;

    // Opening the binary file
    fp = fopen(sockfd, "r");

    if(!fp) {
        perror("ERROR reading from file");
        exit(1);
    }

    // Get the file length
    fseek(fp, 0, SEEK_END);
    file_len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Copy file contents into buffer 
    copy_to_buffer(fp, 0, buffer);

    // Close the file
    fclose(fp);

    return file_len;
}

/* Copy the contents of the file into the buffer string
 * ----------------------------------------------------
 * fp: Pointer to the file object to read from
 * n : Iterator to make sure the buffer is appended to correctly
 * buffer: The char array to copy the binary file into respective string onto
 */ 
void
copy_to_buffer(FILE *fp, int *n, char* buffer) {
    int c;

    // While end of file not reached
    while((c = getc(fp)) != EOF) {
        // Copies each character into the buffer
        buffer[*n++] = c; 
    }
    return;
}

/* Parses the read buffer for the URI of the file requested
 * ---------------------------------------------------------
 * buffer: String/array for the request header to be stored
 * space: The space character in the header
 * 
 * returns: The URI of the file requested as a string
 */
char* 
parse_header(const char *str, const char *space) {
    // Find the pointer to the first space character after request method.
    const char *req_md = strstr(str, space);

    if(req_md != NULL) {
        // Find the pointer to the next space character before the HTTP type.
        const char *http_type = strstr(req_md + 1, space);

        if(http_type != NULL) {
            // Use both pointers to extract the path out.
            const size_t path_len = http_type - (req_md + 1);
            // Allocate Memory to the path string being sliced out.
            char *path = (char*)malloc((path_len + 1) * sizeof(char));

            // Slice the path from the header and copy into the new path string.
            if(path != NULL) {
                memcpy(path, req_md + 1, path_len);
                path[path_len] = '\0';
                return path;
            }
        }
    }

    // If path not returned, handle errors.
    perror("ERROR extracting from header");
    exit(1);

    return NULL;
}


