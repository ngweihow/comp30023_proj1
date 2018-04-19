/*	 Resquest Parser Header File
     +------------------------------------------------------------------+
     | Project Code for Computer Systems COMP30023 2018 S1              |
     | A simple HTML 1.0 Web Server which handles basic GET requests    |
     | To compile: Run the Makefile with "make"                         |
     | Written by: Wei How Ng (828472) wein4                            |
     +------------------------------------------------------------------+

 */

// Struct for matching extension against mimetypes
typedef struct {
    const char *ext;
    const char *mime;
} media_t;


// Function Declarations
char* concat(char* s1, char* s2);
char* parse_header(const char *str, const char *space);
char* find_extension(char* str);
char* match_ext(const char* ext);