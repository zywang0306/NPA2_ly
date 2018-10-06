all: server client

client: client.c sbchead.h
	gcc sbchead.h client.c -o client

server: server_md.c sbchead.h
	gcc sbchead.h server_md.c -o server

clean:
	rm -rf server
	rm -rf client
