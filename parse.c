/*   Resquest Parser Source File
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

#include "parse.h"

#define MEDIA_NUM 4


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
	media_t medias[] = {{".html","text/html"}, {".jpg", "image/jpeg"},
                         {".css", "text/css"}, {".js", "text/javascript"}};


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





