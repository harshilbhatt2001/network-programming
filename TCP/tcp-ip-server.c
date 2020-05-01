/*
Created on Tue Apr 28 22:37:22 2020
@author: Harshil
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define PORT 8080
#define SA struct sockaddr
#define MAX 80


// Function designed for chat between client and server.
void chat(int sockfd) {
    char buff[MAX];
    int n;
    // infinite loop for chat
    while (1) {
        bzero(buff, MAX);

        // read the message from client and copy it in buffer
        read(sockfd, buff, sizeof(buff));
        // print buffer which contains the client contents
        printf("From client: %s\t To client : ", buff);
        bzero(buff, MAX);
        n = 0;
        // copy server message in the buffer
        while ((buff[n++] = getchar()) != '\n')
            ;

        // and send that buffer to client
        write(sockfd, buff, sizeof(buff));

        // if msg contains "Exit" then server exit and chat ended.
        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }
    }
}


int main() {

    int sockfd, connfd, len;
    struct sockaddr_in serveraddr, cli;

    // Create Socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    /*  family: AF_INET -> used to designate type of addresses (IPV4)
        type: SOCK_STREAM -> reliable, 2-way, connection based service
        protocol: 0 -> default
        returns 0 if connection successful
        return -1 if connection fails
    */
    if (sockfd == -1) {
        printf("Socket Creation Failed...\n");
        exit(0);
    } else {
        printf("Socket Creation Successful...\n");
    }

    // Assign IP and PORT
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(PORT);
    /*
        struct sockaddr_in {
            short            sin_family;   // e.g. AF_INET
            unsigned short   sin_port;     // e.g. htons(3490)
            struct in_addr   sin_addr;     // see struct in_addr, below
            char             sin_zero[8];  // zero this if you want to
        };
        The htons() function converts the unsigned short integer from host byte order to network byte order.
        The htonl() function converts the unsigned integer from host byte order to network byte order.
    */

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA*)&serveraddr, sizeof(serveraddr)) != 0)) {
        printf("Socket Binding Failed...\n");
        exit(0);
    }  else {
        printf("Socket Binding Successful...\n");
    }
    /*
        int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
        After creation of the socket, bind function binds the socket to the address and port number specified in addr
    */

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening...\n");
    len = sizeof(cli);
    /*
        int listen(int sockfd, int backlog);
        It puts the server socket in a passive mode, where it waits for the client to approach the server to make a connection.
        The backlog, defines the maximum length to which the queue of pending connections for sockfd may grow.
    */

    // Accept Data from Client
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
        printf("Server Accept Failed...\n");
        exit(0);
    } else {
        printf("Server Accept the Client...\n");
    }
    /*
        int new_socket= accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
        It extracts the first connection request on the queue of pending connections for the listening socket,
        sockfd, creates a new connected socket, and returns a new file descriptor referring to that socket.
    */

    chat(connfd);


    close(sockfd);
}
