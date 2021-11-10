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
#define TCPPORTB "26069"
#define UDPPORTT "21069"
#define UDPPORTS "22069"
#define UDPPORTP "23069"
#define LOCALHOST "127.0.0.1"
#define MAXLEN 512

int client(char *portnum, char *bufA, char *bufB)
{
    struct addrinfo hints;
    struct sockaddr_storage clientaddr;
    struct addrinfo *res;
    int rc, socketfd;
    socklen_t clientlen;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    if ((rc = getaddrinfo(LOCALHOST, portnum, &hints, &res)) != 0)
    {
        fprintf(stderr, "getaddrinfo failed (port %s)\n", portnum);
        return -2;
    }
    /* Create a socket descriptor */
    if ((socketfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0)
    {
        fprintf(stderr, "socket() failed (port %s)\n", portnum);
        return -2;
    }
    if (sendto(socketfd, bufA, strlen(bufA), 0, res->ai_addr, res->ai_addrlen) < 0)
    {
        fprintf(stderr, "recvfrom() failed (port %s)\n", portnum);
        return -2;
    }
    printf("sent: %s\n", bufA);
    if (sendto(socketfd, bufB, strlen(bufB), 0, res->ai_addr, res->ai_addrlen) < 0)
    {
        fprintf(stderr, "recvfrom() failed (port %s)\n", portnum);
        return -2;
    }
    printf("sent: %s\n", bufB);
    clientlen = sizeof(clientaddr);
    char edges[MAXLEN][2][MAXLEN];
    char buf[MAXLEN];
    int i = 0;
    while (1)
    {
        if (recvfrom(socketfd, edges[i], sizeof(edges[i]), 0, res->ai_addr, &(res->ai_addrlen)) < 0)
        {
            fprintf(stderr, "recvfrom() failed (port %s)\n", portnum);
            return -2;
        }
        if (strcmp(edges[i][0], "end") == 0)
        {
            break;
        }

        printf("receive %s, %s\n", edges[i][0], edges[i][1]);
        i++;
    }
    printf("finish reveive\n");

    close(socketfd);
}

int main(int argc, char **argv)
{
    /*clientA*/
    struct addrinfo hintsA;
    struct sockaddr_storage clientaddrA;
    struct addrinfo *resA;
    int rcA, socketfdA;
    socklen_t clientlenA;
    clientlenA = sizeof(clientaddrA);
    memset(&hintsA, 0, sizeof(hintsA));
    hintsA.ai_family = AF_INET;
    hintsA.ai_socktype = SOCK_STREAM;
    int listenfdA;
    if ((rcA = getaddrinfo(LOCALHOST, TCPPORTA, &hintsA, &resA)) != 0)
    {
        fprintf(stderr, "getaddrinfo failed (port %s)\n", TCPPORTA);
        return -2;
    }
    if ((socketfdA = socket(resA->ai_family, resA->ai_socktype, resA->ai_protocol)) < 0)
    {
        fprintf(stderr, "socket() failed (port %s)\n", TCPPORTA);
        return -2;
    }
    /* Bind the descriptor to the port */
    if (bind(socketfdA, resA->ai_addr, resA->ai_addrlen) < 0)
    {
        fprintf(stderr, "bind() failed (port %s)\n", TCPPORTA);
        return -2;
    }
    if ((rcA = listen(socketfdA, 20)) < 0)
    {
        fprintf(stderr, "listen() failed (port %s)\n", TCPPORTA);
        return -2;
    }
    /*clientA end*/

    /*clientB*/
    struct addrinfo hintsB;
    struct sockaddr_storage clientaddrB;
    struct addrinfo *resB;
    int rcB, socketfdB;
    socklen_t clientlenB;
    int listenfdB;
    clientlenB = sizeof(clientaddrB);
    memset(&hintsB, 0, sizeof(hintsB));
    hintsB.ai_family = AF_INET;
    hintsB.ai_socktype = SOCK_STREAM;
    if ((rcB = getaddrinfo(LOCALHOST, TCPPORTB, &hintsB, &resB)) != 0)
    {
        fprintf(stderr, "getaddrinfo failed (port %s)\n", TCPPORTB);
        return -2;
    }
    /* Create a socket descriptor */
    if ((socketfdB = socket(resB->ai_family, resB->ai_socktype, resB->ai_protocol)) < 0)
    {
        fprintf(stderr, "socket() failed (port %s)\n", TCPPORTB);
        return -2;
    }
    /* Bind the descriptor to the port */
    if (bind(socketfdB, resB->ai_addr, resB->ai_addrlen) < 0)
    {
        fprintf(stderr, "bind() failed (port %s)\n", TCPPORTB);
        return -2;
    }
    if ((rcB = listen(socketfdB, 20)) < 0)
    {
        fprintf(stderr, "listen() failed (port %s)\n", TCPPORTB);
        return -2;
    }
    /*clientB end*/

    printf("The Central server is up and running.\n");

    /*start listen*/
    while (1)
    {
        int recvA, recvB;
        char bufA[MAXLEN] = "";
        char bufB[MAXLEN] = "";
        /*accept for A*/
        if ((listenfdA = accept(socketfdA, (struct sockaddr *)&clientaddrA, &clientlenA)) < 0)
        {
            fprintf(stderr, "listen() failed (port %s)\n", TCPPORTA);
            return -2;
        }
        /*accept for B*/
        if ((listenfdB = accept(socketfdB, (struct sockaddr *)&clientaddrB, &clientlenB)) < 0)
        {
            fprintf(stderr, "listen() failed (port %s)\n", TCPPORTB);
            return -2;
        }

        // inet_ntop(res->ai_family, (struct sockaddr *)&clientaddr, ipstr, sizeof(ipstr));
        // printf("Get connected with %s\n", ipstr);

        /*receive A*/
        if (recvA = recv(listenfdA, bufA, MAXLEN - 1, 0) < 0)
        {
            fprintf(stderr, "listen() failed (port %s)\n", TCPPORTA);
            return -2;
        }
        else
        {
            printf("The Central server received input=\"%s\" from the client using TCP over port %s.\n", bufA, TCPPORTA);
        }
        /*end receive A*/

        /*receive B*/
        if (recvB = recv(listenfdB, bufB, MAXLEN - 1, 0) < 0)
        {
            fprintf(stderr, "listen() failed (port %s)\n", TCPPORTB);
            return -2;
        }
        else
        {
            printf("The Central server received input=\"%s\" from the client using TCP over port %s.\n", bufB, TCPPORTB);
        }
        /*end receive B*/

        /*connect to serverT*/
        client(UDPPORTT, bufA, bufB);
        memset(bufA, 0, sizeof(bufA));
        memset(bufB, 0, sizeof(bufB));
        close(listenfdA);
        close(listenfdB);
        /* end connect to serverT*/
    }
    /*end listen*/
}