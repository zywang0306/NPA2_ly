all: client
client: client.c sbchead.h
	gcc -Wall client.c sbchead.h -o client

#client: client.o 
#	gcc -o client client.o
#server.o: 
#	gcc -Wall -o  server  server.c
clean:
#	rm -rf server
#	rm -rf client
