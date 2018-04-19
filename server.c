/*   Main Server File
     +------------------------------------------------------------------+
     | Project Code for Computer Systems COMP30023 2018 S1              |
     | A simple HTML 1.0 Web Server which handles basic GET requests    |
     | To compile: Run the Makefile with "make"                         |
     | Written by: Wei How Ng (828472) wein4                            |
     +------------------------------------------------------------------+

 */


// Library and header includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>

// Importing header files
#include "parse.h"
#include "read.h"

// Constant Declarations and Initialisation
#define BUFFERSIZE 8192
#define SPACE " "

// Resquest Reponses
const char* res200 = "HTTP/1.0 200 OK\r\nContent-Type: %s\r\n\r\n";
const char* res404 = "HTTP/1.0 404 Not Found\r\nContent-Type: %s\r\n\r\n";

// -------------------
// Struct Definition

// Struct for storing thread arguments
typedef struct {
    int sockfd;
    char* root_path;
} thread_arg_t;

// -------------------
// Function Declarations 
void* thread_activity(void* arg);                         
void print_res(int sockfd, char* response,char* file, int bytes, char* mimetype);


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

        // Parsing newsockfd and the root directory into a thread_arg_t struct
        thread_arg_t thread_arg;
        thread_arg.sockfd = newsockfd;
        thread_arg.root_path = argv[2];

        // Create pthread **HERE**
        
        pthread_t tid;
        pthread_attr_t attr;
        pthread_attr_init(&attr);

        int error_c = pthread_create(&tid, &attr,
                                     thread_activity,
                                     &thread_arg);
        if(error_c) {
            perror("ERROR on creating thread");
            exit(1);
        }
        

        // Detach pthread **HERE**
        int error_d = pthread_detach(tid);
        if(error_d) {
            perror("ERROR on detaching thread");
            exit(1);
        }
    }
    

    // ---------------------------------------
    return 0;
}

//--------------------------------------------------------------------------------------------------
/* Void Pointer Function 
 * ---------------------
 * Handles the activities each thread executes
 */

void* thread_activity(void* thread_arg) {

    /* Handle formatting of the void argument
       struct into normal arguments*/
    
    thread_arg_t* thread_arg_ptr = (thread_arg_t*) thread_arg;
    int newsockfd = thread_arg_ptr->sockfd;
    char* root = thread_arg_ptr->root_path;
    
    // --------------
    // Processes start here

    // Array used for header buffer
    char* header_buffer;
    char response_buffer[BUFFERSIZE];

    // Allocate Space to the buffer for the header
    // memset(&header_buffer, '0', (BUFFERSIZE*sizeof(char)));
    header_buffer = malloc(sizeof(char) * BUFFERSIZE);

    // Read characters from the socket and then process them
    int n = read(newsockfd, header_buffer, BUFFERSIZE-1);

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
    char* abs_path = concat(root, relative_path);
    free(relative_path);


    // Create a buffer info struct and default the length to 0
    buffer_info bf;
    bf.buff_len = 0;
    
    // Find the extension and its respective mime type
    char* ext = find_extension(abs_path);
    char* mimetype = match_ext(ext);
    

    // ---------------------------------------
    /* Writing*/

    /* Writing the response - parse the correct content type into
     the respective response and write it into the socket. */

    // Check that the file exists first
    int res_int = check_file_exist(abs_path);
    if (res_int) {
        // Handle the response header formatting
        sprintf(response_buffer, res200, mimetype);
        

        // Open the file located at the abs_path
        read_file(abs_path,&bf);    
        print_res(newsockfd, (response_buffer), bf.buff_str, bf.buff_len, mimetype);  

        // Free the buffer_info struct
        free(bf.buff_str);
    }   

    // Print response if not found
    else {
        // Handle the response header formatting.
        sprintf(response_buffer, res404, mimetype);

        // Write to socket.
        print_res(newsockfd, (response_buffer), NULL, 0, mimetype);
    }


    // Free abs_path 
    free(abs_path);

    // Free mimetype
    if (mimetype != NULL) {
        free(mimetype);    
    }

    return NULL;

}


//--------------------------------------------------------------------------------------------------
/* Helper Functions
 *
 */

/* Write the responses to the socket
 * --------------------------------
 * sockfd: The new socket file descriptor passed in for every resquest
 * response: The response type the response should take depending on the file
 * file: The file stored in the buffer if there is one
 * bytes: The length of the file written in bytes
 */
void 
print_res(int sockfd, char* response, char* file , int bytes, char* mimetype) {

    // Check the type of response coming in.
    if(bytes) {
        // If file found and is not empty.
        write(sockfd, response, strlen(response));
        write(sockfd,file,bytes);
    }

    // If file was not found, just write response
    else{
        write(sockfd, response, strlen(response));
    }

    close(sockfd);
}

