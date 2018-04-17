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
#include <pthread.h>

// Constant Declarations and Initialisation
#define BUFFERSIZE 8192
#define SPACE " "
#define MEDIA_NUM 4

// Resquest Reponses
const char* res200 = "HTTP/1.0 200 OK\r\nContent-Type: %s\r\n\r\n";
const char* res404 = "HTTP/1.0 404 Not Found\r\nContent-Type: %s\r\n\r\n";

// -------------------
// Struct Definition
// Struct for storing buffer and it's length
typedef struct {
    char* buff_str;
    unsigned long buff_len;
} buffer_info;

// Struct for matching extension against mimetypes
typedef struct {
    const char *ext;
    const char *mime;
} media_t;

// Struct for storing thread arguments
typedef struct {
    int sockfd;
    char* root_path;
} thread_arg_t;



// Global Constants
const media_t medias[] = {{".html","text/html"}, {".jpg", "image/jpeg"},
                         {".css", "text/css"}, {".js", "text/javascript"}};


// Function Declarations 
void* thread_activity(void* arg);                         
char* concat(char* s1, char* s2);
void print_res(int sockfd, char* response,char* file, int bytes, char* mimetype);
void read_file(char* path, buffer_info* bf);
void copy_to_buffer(FILE *fp, int *n, char* buffer);
char* parse_header(const char *str, const char *space);
char* find_extension(char* str);
char* match_ext(const char* ext);
int check_file_exist(char* path);

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
        
        //thread_activity(&thread_arg);

        // Detach pthread **HERE**
        int error_d = pthread_detach(tid);
        if(error_d) {
            perror("ERROR on detaching thread");
            exit(1);
        }

        //free(thread_arg.root_path);
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

    // Free 



    return NULL;

}


//--------------------------------------------------------------------------------------------------
/* Helper Functions
 *
 */



/* Concatenate function to appending one string to another 
 * -------------------------------------------------------
 * s1: First String to be at the front.
 * s2: Second String to be at the back.
 *
 * return: Pointer to the concatenated string.
 */
char*
concat(char* s1, char* s2) {   
    // Determine the length of the output and allocating memory for it.
    int output_len = strlen(s1) + strlen(s2) + 1;
    char* concat_str = malloc(output_len * sizeof(char));

    // Copy the first string into the allocated return string.
    strcpy(concat_str, s1);
    // Concat the second on onto the allocated return string afterwards.
    strcat(concat_str, s2);

    return concat_str;
}


/* Write the responses to the socket
 * --------------------------------
 * sockfd: The new socket file descriptor passed in for every resquest
 * response: The response type the response should take depending on the file
 * file: The file stored in the buffer if there is one
 * bytes: The length of the file written in bytes
 */
void 
print_res(int sockfd, char* response, char* file , int bytes, char* mimetype) {
    //printf("%d\n", sockfd);
    //printf("%s\n", response);
    //printf("%s\n", file);
    //printf("%d\n", bytes);
    //printf("%s\n", mimetype);

    // Check the type of response coming in.
    if(bytes) {
        // If file found and is not empty.
        write(sockfd, response, strlen(response));
        write(sockfd,file,bytes);
    }

    else{
        write(sockfd, response, strlen(response));
    }

    close(sockfd);
}

/* File Reading function to parse the GET request
 * ----------------------------------------------
 * sockfd: The socket in which the file is being read from.
 * buffer: The string/array where the binary file is being read onto.
 *
 * return: The number of bytes the file contains (that was read into buffer).
 */
void
read_file(char* path, buffer_info* bf) {
    FILE *fp;
    unsigned long file_len;
    char* buffer;

    // Opening the file.
    fp = fopen(path, "r");

    // Handle Errors 
    if(!fp) {
        perror("ERROR reading from file");
        exit(1);
    }

    // Get the file length.
    fseek(fp, 0, SEEK_END);
    file_len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Allocate memory to the buffer.
    buffer = malloc(file_len * sizeof(char));
    memset(buffer, '\0', file_len);

    /* Using fread */
    fread(buffer, 1, file_len, fp);

    /* Using Manual Reading */
    /*
    int n = 0;

    // Copy file contents into buffer.
    int c;

    // While end of file not reached.
    while((c = getc(fp)) != EOF) {
        //printf("%c",c);
        //fflush(stdout);
        // Copies each character into the buffer.
        buffer[n++] = c; 

    }

    */
    //copy_to_buffer(fp, &n, buffer);

    // Close the file.
    fclose(fp);

    bf->buff_str = buffer;
    bf->buff_len = file_len;
}

/* Copy the contents of the file into the buffer string
 * ----------------------------------------------------
 * fp: Pointer to the file object to read from.
 * n : Iterator to make sure the buffer is appended to correctly.
 * buffer: The char array to copy the binary file into respective string onto.
 */ 
void
copy_to_buffer(FILE *fp, int *n, char* buffer) {

   
}

/* Parses the read buffer for the URI of the file requested
 * ---------------------------------------------------------
 * buffer: String/array for the request header to be stored.
 * space: The space character in the header.
 *
 * returns: The URI of the file requested as a string.
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

/* Find the extension and store there aside
 * ------------------------------------------
 * str: The request header parsed and with only abs_path.
 *
 * return: The extension string.
 */
char*
find_extension(char* str) {
    const char dot = '.';
    char* ext;
    //int ext_len;

    // Get extension using strrchar.
    ext = strrchr(str, dot);
    
    // Return the extension as string.
    return ext;   
}


/* Match the extension to the respective response type
 * ---------------------------------------------------
 * ext: constant string of the file extension.
 *
 * return: the string value of the media type ready to be appended.
 */
char*
match_ext(const char* ext) {

    // Defining the matched media type string.
    char* media = malloc(sizeof(char) * 256);
    int i;

    // Loop through possible media types and return the right one.
    for(i=0;i<MEDIA_NUM;i++) {

        // If the string extension matches one of the possibilities.
        if(!(strcmp(medias[i].ext, ext))) {
            strcpy(media, medias[i].mime);
            return media;
        }
    }

    // I know I shouldn't be returning null but...
    return NULL;
}


/* Check if the file exists
 * ------------------------
 * path: The absolute path of the file passed as a string 
 *
 * return: The reponse code required to handle the file
 */
int 
check_file_exist(char* path) {
    // If file does not exists...
    if(access(path, F_OK) < 0) {
        return 0;
    }
    // If file does exists...
    return 1;
}