#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#define TCPPORTA "25069"
#define LOCALHOST "127.0.0.1"
#define MAXLEN 512

int main(int argc, char **argv)
{
    struct addrinfo hints;
    struct sockaddr_storage clientaddr;
    struct addrinfo *res;
    int rc, socketfd;
    char buf[MAXLEN];
    socklen_t clientlen;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if ((rc = getaddrinfo(LOCALHOST, TCPPORTA, &hints, &res)) != 0)
    {
        fprintf(stderr, "getaddrinfo failed (port %s)\n", TCPPORTA);
        return -2;
    }
    /* test the current IP */
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
    void *addr = &(ipv4->sin_addr);
    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(res->ai_family, addr, ipstr, sizeof(ipstr));
    printf("%s\n", ipstr);
    /* Create a socket descriptor */
    if ((socketfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0)
    {
        fprintf(stderr, "socket() failed (port %s)\n", TCPPORTA);
        return -2;
    }
    /* Bind the descriptor to the port */
    if (bind(socketfd, res->ai_addr, res->ai_addrlen) < 0)
    {
        fprintf(stderr, "bind() failed (port %s)\n", TCPPORTA);
        return -2;
    }
    if ((rc = listen(socketfd, 20)) < 0)
    {
        fprintf(stderr, "listen() failed (port %s)\n", TCPPORTA);
        return -2;
    }
    printf("The Central server is up and running\n");
    while (1)
    {
        int listenfd;
        clientlen = sizeof(clientaddr);
        if ((listenfd = accept(socketfd, (struct sockaddr *)&clientaddr, &clientlen)) < 0)
        {
            fprintf(stderr, "listen() failed (port %s)\n", TCPPORTA);
            return -2;
        }
        inet_ntop(res->ai_family, (struct sockaddr *)&clientaddr, ipstr, sizeof(ipstr));
        printf("Get connected with %s\n", ipstr);
        if (recv(listenfd, buf, MAXLEN - 1, 0) < 0)
        {
            fprintf(stderr, "listen() failed (port %s)\n", TCPPORTA);
            return -2;
        }
        printf("Receive: %s\n", buf);
    }
}