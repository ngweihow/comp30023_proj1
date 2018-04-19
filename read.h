/*   File reader header file
     +------------------------------------------------------------------+
     | Project Code for Computer Systems COMP30023 2018 S1              |
     | A simple HTML 1.0 Web Server which handles basic GET requests    |
     | To compile: Run the Makefile with "make"                         |
     | Written by: Wei How Ng (828472) wein4                            |
     +------------------------------------------------------------------+

 */

// Struct for storing buffer and it's length
typedef struct {
    char* buff_str;
    unsigned long buff_len;
} buffer_info;

// Function Declarations
void read_file(char* path, buffer_info* bf);
int check_file_exist(char* path);