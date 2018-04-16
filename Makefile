server: server.c
	gcc -Wall -o server server.c
clean :
	rm server

scp: 
	scp *.c *.h Makefile ubuntu@115.146.85.92: 
