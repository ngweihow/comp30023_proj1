server_real: server_real.c
	gcc -Wall -o server_real server_real.c
clean :
	rm server_real

scp: 
	scp *.c *.h Makefile ubuntu@115.146.85.92: 