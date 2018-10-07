# Network Programming Assignment 2

* All of this part is implemented by Yu Lei.

* How to run:
    *  Enter "make -f Makefile" to compile the code.
    *  Use the command "./server <server.ip> <port.no> <max_clients>" to open the server.
    *  Open another Terminal and use the command "./client <username> <server.ip> <server.port>" to create a client.
    *  

* This part contains the client that can join to the server, read the message from the server, and send message to the server.
* Also, there are a model server for testing.
*
* sbchead.h is the headfile defines the data struct and some static constants.
* join() – The function implements JOIN. 
* send_MSG() – When SELECT find that there are inputs from the keyboard, then will send them to the server. (SEND)
* read_MSG() - When SELECT find that there are inputs from the server, then will read them and display. (FWD)
* 
* Group distribution: 
    * Yulei writes the client part, and a incomplete server part for testing.