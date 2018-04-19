/*   File Reader Source File
     +------------------------------------------------------------------+
     | Project Code for Computer Systems COMP30023 2018 S1              |
     | A simple HTML 1.0 Web Server which handles basic GET requests    |
     | To compile: Run the Makefile with "make"                         |
     | Written by: Wei How Ng (828472) wein4                            |
     +------------------------------------------------------------------+

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "read.h"



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
    
    fclose(fp);

    bf->buff_str = buffer;
    bf->buff_len = file_len;
}
