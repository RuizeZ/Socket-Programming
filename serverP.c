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
#define UDPPORT "23069"
#define LOCALHOST "127.0.0.1"
#define MAXLEN 512

int main(int argc, char **argv)
{
    struct addrinfo hints;
    struct sockaddr_storage clientaddr;
    struct addrinfo *res;
    int rc, socketfd;
    socklen_t clientlen;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    if ((rc = getaddrinfo(LOCALHOST, UDPPORT, &hints, &res)) != 0)
    {
        fprintf(stderr, "getaddrinfo failed (port %s)\n", UDPPORT);
        return -2;
    }
    printf("The serverP is up and running using UDP on port %s.\n", UDPPORT);
    while (1)
    {
        /* Create a socket descriptor */
        if ((socketfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0)
        {
            fprintf(stderr, "socket() failed (port %s)\n", UDPPORT);
            return -2;
        }
        /* Bind the descriptor to the port */
        if (bind(socketfd, res->ai_addr, res->ai_addrlen) < 0)
        {
            fprintf(stderr, "bind() failed (port %s)\n", UDPPORT);
            return -2;
        }
        /*receive edges from client*/
        clientlen = sizeof(clientaddr);
        char edges[MAXLEN][2][MAXLEN];
        int edgeInx = 0;
        while (1)
        {
            if (recvfrom(socketfd, edges[edgeInx], sizeof(edges[edgeInx]), 0, (struct sockaddr *)&clientaddr, &clientlen) < 0)
            {
                fprintf(stderr, "recvfrom() failed (port %s)\n", UDPPORT);
                return -2;
            }
            if (strcmp(edges[edgeInx][0], "end") == 0)
            {
                break;
            }
            printf("receive %s, %s\n", edges[edgeInx][0], edges[edgeInx][1]);
            edgeInx++;
        }
        /*end receive edges*/

        /*receive scores from client*/
        char scoresneeded[MAXLEN][2][MAXLEN];
        int scoresneededsize = 0;
        while (1)
        {
            if (recvfrom(socketfd, scoresneeded[scoresneededsize], sizeof(scoresneeded[scoresneededsize]), 0, (struct sockaddr *)&clientaddr, &clientlen) < 0)
            {
                fprintf(stderr, "recvfrom() failed (port %s)\n", UDPPORT);
                return -2;
            }
            if (strcmp(scoresneeded[scoresneededsize][0], "end") == 0)
            {
                break;
            }
            printf("receive %s, %s\n", scoresneeded[scoresneededsize][0], scoresneeded[scoresneededsize][1]);
            scoresneededsize++;
        }
        /*end receive scores from client*/

        // inet_ntop(res->ai_family, (struct sockaddr *)&clientaddr, ipstr, sizeof(ipstr));
        // printf("Get connected with %s\n", ipstr);
        char buf[MAXLEN] = "";

        if (recvfrom(socketfd, buf, MAXLEN - 1, 0, (struct sockaddr *)&clientaddr, &clientlen) < 0)
        {
            fprintf(stderr, "recvfrom() failed (port %s)\n", UDPPORT);
            return -2;
        }
        printf("The ServerP received a request from Central to get the topology.\n");
        close(socketfd);
    }
}