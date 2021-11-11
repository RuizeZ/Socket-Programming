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
#define UDPPORT "22069"
#define LOCALHOST "127.0.0.1"
#define MAXLEN 512
int findscore()
{
    int nextC;
    int isName = 1;
    char name[MAXLEN] = "", score[MAXLEN] = "";
    char scores[MAXLEN][2][MAXLEN];
    int scoresSize = 0;
    char prevChar = ' ';
    FILE *fp;
    fp = fopen("scores.txt", "r");
    /*find all score in the file, put them in the list scores[][]*/
    do
    {
        nextC = getc(fp);
        if (nextC != EOF)
        {
            if ((char)nextC != '\n')
            {
                if (nextC != ' ')
                {
                    char newChar = nextC;
                    if (isName)
                    {
                        /*add current char to the string*/
                        strncat(name, &(newChar), 1);
                    }
                    else
                    {
                        /*add current char to the string*/
                        strncat(score, &(newChar), 1);
                    }
                }
                else
                {
                    /*start to read score*/
                    isName = 0;
                }
            }
            else
            {
                /*if \n means starting next line, increasing edgeInx so that use a new string to store next line*/
                strcpy(scores[scoresSize][0], name);
                strcpy(scores[scoresSize][1], score);
                scoresSize++;
                memset(name, 0, sizeof(name));
                memset(score, 0, sizeof(score));
                isName = 1;
            }
        }
        /*reach the end of the file*/
        else
        {
            if (prevChar != '\n')
            {
                /*there is no empty line in the file, decrease the edgeInx*/
                strcpy(scores[scoresSize][0], name);
                strcpy(scores[scoresSize][1], score);
                scoresSize++;
                memset(name, 0, sizeof(name));
                memset(score, 0, sizeof(score));
                isName = 1;
            }
        }
        /*revord the current char as prev in order to check empty line at the end*/
        prevChar = (char)nextC;
    } while (nextC != EOF);
    /*end of reading file*/
    for (int i = 0; i < scoresSize; i++)
    {
        printf("read %s, %s\n", scores[i][0], scores[i][1]);
    }
}
int main(int argc, char **argv)
{
    findscore();
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
    printf("The serverS is up and running using UDP on port %s.\n", UDPPORT);
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
        /*receive data from client*/
        clientlen = sizeof(clientaddr);
        char edges[MAXLEN][2][MAXLEN];
        int i = 0;
        while (1)
        {
            if (recvfrom(socketfd, edges[i], sizeof(edges[i]), 0, (struct sockaddr *)&clientaddr, &clientlen) < 0)
            {
                fprintf(stderr, "recvfrom() failed (port %s)\n", UDPPORT);
                return -2;
            }
            if (strcmp(edges[i][0], "end") == 0)
            {
                break;
            }
            printf("receive %s, %s\n", edges[i][0], edges[i][1]);
            i++;
        }
        printf("The ServerS received a request from Central to get the topology.\n");
        /*end receive data*/
        close(socketfd);
    }
}