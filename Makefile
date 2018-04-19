#     Standard Make File
#     +------------------------------------------------------------------+
#     | Project Code for Computer Systems COMP30023 2018 S1              |
#     | A simple HTML 1.0 Web Server which handles basic GET requests    |
#     | To compile: Run the Makefile with "make"                         |
#     | Written by: Wei How Ng (828472) wein4                            |
#     +------------------------------------------------------------------+


server: server.c read.c parse.c
	gcc -Wall -pthread -o server server.c read.c parse.c

clean: 
	rm server

scp: 
	scp *.c *.h Makefile ubuntu@115.146.85.92:./comp30023_p1/
