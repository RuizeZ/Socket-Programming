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
#define LOCALHOST "127.0.0.1"
#define CENTRALPORT "26069"
#define MAXLEN 512
int main(int argc, char **argv)
{
    struct addrinfo hints;
    struct sockaddr_storage clientaddr;
    struct addrinfo *res;
    int rc, clientfd;
    socklen_t clientlen;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    char *msg;
    if (argc == 2)
    {
        printf("input: %s\n", argv[1]);
        msg = argv[1];
        printf("input: %s\n", msg);
    }

    if ((rc = getaddrinfo(LOCALHOST, CENTRALPORT, &hints, &res)) != 0)
    {
        fprintf(stderr, "getaddrinfo failed (port %s)\n", CENTRALPORT);
        return -2;
    }
    /* test the current IP */
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
    void *addr = &(ipv4->sin_addr);
    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(res->ai_family, addr, ipstr, sizeof(ipstr));
    printf("%s\n", ipstr);
    /* Create a socket descriptor */
    if ((clientfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0)
    {
        fprintf(stderr, "socket() failed (port %s)\n", CENTRALPORT);
        return -2;
    }
    printf("The client is up and running.\n");
    if (connect(clientfd, res->ai_addr, res->ai_addrlen) < 0)
    {
        fprintf(stderr, "connect() failed (port %s)\n", CENTRALPORT);
        return -2;
    }
    inet_ntop(res->ai_family, (struct sockaddr *)&clientaddr, ipstr, sizeof(ipstr));
    printf("Connected with %s\n", ipstr);
    if (send(clientfd, msg, strlen(msg), 0) == -1)
    {
        fprintf(stderr, "send() failed (port %s)\n", CENTRALPORT);
    }
    printf("The client sent %s to the Central server.\n", msg);
    /*receive result from central*/
    char finalPath[MAXLEN][MAXLEN];
    int finalPathSize = 0;
    char finalscorechar[MAXLEN];
    char *eptr;
    double finalscore;
    while (1)
    {
        if (recv(clientfd, finalPath[finalPathSize], sizeof(finalPath[finalPathSize]), 0) < 0)
        {
            fprintf(stderr, "recv() failed (port %s)\n", CENTRALPORT);
            return -2;
        }
        if (strcmp(finalPath[finalPathSize], "end") == 0)
        {
            break;
        }
        finalPathSize++;
    }
    if (finalPathSize == 1)
    {
        printf("Found no compatibility between %s and %s\n", msg, finalPath[0]);
    }
    else
    {
        finalscore = strtod(finalPath[finalPathSize - 1], &eptr);
        for (int i = 0; i < finalPathSize; i++)
        {
            printf("receive: %s\n", finalPath[i]);
        }
        if (strcmp(finalPath[0], msg) == 0)
        {
            for (int i = 0; i < finalPathSize - 1; i++)
            {
                printf("%s", finalPath[i]);
                if (i < finalPathSize - 2)
                {
                    printf(" --- ");
                }
                else
                {
                    printf("\n");
                }
            }
        }
        else
        {
            for (int i = finalPathSize - 2; i >= 0; i--)
            {
                printf("%s", finalPath[i]);
                if (i > 0)
                {
                    printf(" --- ");
                }
                else
                {
                    printf("\n");
                }
            }
        }
        printf("Compatibility score: %.2f\n", finalscore);
    }
    close(clientfd);
}