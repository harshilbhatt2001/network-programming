/*
Created on Tue Apr 28 21:24:22 2020
@author: Harshil
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#define PORT 8080
#define SA struct sockaddr
#define MAX 80

void chat(int sockfd)
{
    char buff[MAX];
    int n;
    while (1) {
        bzero(buff, sizeof(buff));
        printf("Enter the string : ");
        n = 0;
        while ((buff[n++] = getchar()) != '\n')
            ;
        write(sockfd, buff, sizeof(buff));
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));
        printf("From Server : %s", buff);
        if ((strncmp(buff, "exit", 4)) == 0) {
            printf("Client Exit...\n");
            break;
        }
    }
}


int main() {

    int sockfd, connfd;
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

    bzero(&serveraddr, sizeof(serveraddr));
    /*
        bzero(*s, n)
        The bzero() function erases the data in the n bytes of the memory
        starting at the location pointed to by s, by writing zeros (bytes
        containing '\0') to that area.
    */

    // Assign IP and PORT
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serveraddr.sin_port = htons(PORT);
    /*
        struct sockaddr_in {
            short            sin_family;   // e.g. AF_INET
            unsigned short   sin_port;     // e.g. htons(3490)
            struct in_addr   sin_addr;     // see struct in_addr, below
            char             sin_zero[8];  // zero this if you want to
        };
        The inet_addr() function shall convert the string in the standard IPv4 dotted decimal notation,
        to an integer value suitable for use as an Internet address.
        The htons() function converts the unsigned short integer from host byte order to network byte order.
    */

    // Connect Client and Server
    if (connect(sockfd, (SA*)&serveraddr, sizeof(serveraddr)) != 0) {
        printf("Connection To Server Failes...\n");
        exit(0);
    } else {
        printf("Connected to Server\n");
    }
    /*
        int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

        The connect() system call connects the socket referred to by the file
        descriptor sockfd to the address specified by sockaddr_in.  The addrlen
        argument specifies the size of addr.
    */

    chat(sockfd);

    close(sockfd);

}
