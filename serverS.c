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
int findscore(char edges[][2][MAXLEN], char scoresneeded[][2][MAXLEN], int edgeInx)
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
    // for (int i = 0; i < scoresSize; i++)
    // {
    //     printf("read %s, %s\n", scores[i][0], scores[i][1]);
    // }
    /*find all scores needed*/
    int selected[scoresSize];
    int scoresneededSize = 0;
    int seletedNum = 0;
    memset(selected, 0, sizeof(selected));
    for (int i = 0; i < edgeInx; i++)
    {
        int breakNum = 0;
        for (int j = 0; i < scoresSize; j++)
        {
            /*find all score needed for each name*/
            /*if find first name*/
            if (strcmp(scores[j][0], edges[i][0]) == 0)
            {
                if (selected[j] == 0)
                { /*copy this name and score to scoresneeded array*/
                    strcpy(scoresneeded[scoresneededSize][0], scores[j][0]);
                    strcpy(scoresneeded[scoresneededSize][1], scores[j][1]);
                    selected[j] = 1;
                    seletedNum++;
                    scoresneededSize++;
                }
                breakNum++;
            }
            if (strcmp(scores[j][0], edges[i][1]) == 0)
            {
                if (selected[j] == 0)
                { /*copy this name and score to scoresneeded array*/
                    strcpy(scoresneeded[scoresneededSize][0], scores[j][0]);
                    strcpy(scoresneeded[scoresneededSize][1], scores[j][1]);
                    selected[j] = 1;
                    seletedNum++;
                    scoresneededSize++;
                }
                breakNum++;
            }
            if (breakNum == 2)
            {
                break;
            }
        }
        if (seletedNum == scoresSize)
        {
            break;
        }
    }
    /*end find all scores needed*/

    // for (int i = 0; i < scoresneededSize; i++)
    // {
    //     printf("selected %s, %s\n", scoresneeded[i][0], scoresneeded[i][1]);
    // }
    return scoresneededSize;
}
int main(int argc, char **argv)
{
    struct addrinfo hints;
    struct sockaddr_storage clientaddr;
    struct addrinfo *res;
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    int portS = 0;
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
        if (getsockname(socketfd, (struct sockaddr *)&sin, &len) == -1)
        {
            perror("getsockname");
        }
        else
        {
            portS = ntohs(sin.sin_port);
        }
        printf("The serverS is up and running using UDP on port %d.\n", portS);

        /*receive data from client*/
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
        printf("The ServerS received a request from Central to get the topology.\n");
        /*end receive data*/
        /*find all needed score for names*/
        char scoresneeded[MAXLEN][2][MAXLEN];
        int scoresneededSize = findscore(edges, scoresneeded, edgeInx);
        printf("1\n");
        /*end find all needed score for names*/
        /*send all edges back to central*/
        for (int i = 0; i < scoresneededSize; i++)
        {
            if (sendto(socketfd, scoresneeded[i], sizeof(scoresneeded[i]), 0, (struct sockaddr *)&clientaddr, clientlen) < 0)
            {
                fprintf(stderr, "sendto() failed (port %s)\n", UDPPORT);
                return -2;
            }
        }
        char end[] = "end";
        if (sendto(socketfd, end, sizeof(end), 0, (struct sockaddr *)&clientaddr, clientlen) < 0)
        {
            fprintf(stderr, "sendto() failed (port %s)\n", UDPPORT);
            return -2;
        }
        /*end send all edges back to central*/
        printf("The ServerS finished sending the scores to Central.\n");
        close(socketfd);
    }
}