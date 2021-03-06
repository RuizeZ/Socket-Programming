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
#define UDPPORT "21069"
#define LOCALHOST "127.0.0.1"
#define MAXLEN 512

void bfs(char edges[][2][MAXLEN], int edgeInx, char names[][MAXLEN], char queue[][MAXLEN], int queueSize, int *visited, int visitedNum, int queueInx);
int findEdge(char names[][MAXLEN], char edges[][2][MAXLEN], int *visited);

/*find all edges in the file*/
int findEdge(char names[][MAXLEN], char edges[][2][MAXLEN], int *visited)
{
    int edgeInx = 0;
    FILE *fp;
    fp = fopen("edgelist.txt", "r");
    int nextC;
    char prevChar = ' ';
    char firstName[MAXLEN] = "", secondName[MAXLEN] = "";
    int isFirstName = 1;

    /*find all edges in the file, put them in the list deges[][]*/
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
                    if (isFirstName)
                    {
                        /*add current char to the string*/
                        strncat(firstName, &(newChar), 1);
                    }
                    else
                    {
                        /*add current char to the string*/
                        strncat(secondName, &(newChar), 1);
                    }
                }
                else
                {
                    /*start to read second name*/
                    isFirstName = 0;
                }
            }
            else
            {
                /*if \n means starting next line, increasing edgeInx so that use a new string to store next line*/
                strcpy(edges[edgeInx][0], firstName);
                strcpy(edges[edgeInx][1], secondName);
                edgeInx++;
                memset(firstName, 0, sizeof(firstName));
                memset(secondName, 0, sizeof(secondName));
                isFirstName = 1;
            }
        }
        /*reach the end of the file*/
        else
        {
            if (prevChar != '\n')
            {
                /*there is a empty line in the file, decrease the edgeInx*/
                strcpy(edges[edgeInx][0], firstName);
                strcpy(edges[edgeInx][1], secondName);
                edgeInx++;
                memset(firstName, 0, sizeof(firstName));
                memset(secondName, 0, sizeof(secondName));
                isFirstName = 1;
            }
        }
        /*revord the current char as prev in order to check empty line at the end*/
        prevChar = (char)nextC;
    } while (nextC != EOF);
    /*end of reading file*/

    /*find all edges that are associated with input name*/
    char queue[edgeInx * 2][MAXLEN];
    memset(queue, 0, sizeof(queue));
    int queueSize = 0, queueInx = 0, visitedNum = 0;
    strcpy(queue[0], names[0]);
    queueSize++;
    bfs(edges, edgeInx, names, queue, queueSize, visited, visitedNum, queueInx);
    return edgeInx;
}

// BFS algorithm
void bfs(char edges[][2][MAXLEN], int edgeInx, char names[][MAXLEN], char queue[][MAXLEN], int queueSize, int *visited, int visitedNum, int queueInx)
{

    while (queueInx != queueSize)
    {
        char currName[MAXLEN];
        strcpy(currName, *(queue + queueInx));
        // printf("current name is %s\n", currName);
        queueInx++;
        for (int i = 0; i < edgeInx; i++)
        {
            /*if current edge contains currName*/
            if (strcmp(edges[i][0], currName) == 0 && !visited[i])
            {
                visited[i] = 1;
                visitedNum++;
                strcpy(queue[queueSize], edges[i][1]);
                queueSize++;
            }
            else if (strcmp(edges[i][1], currName) == 0 && !visited[i])
            {
                visited[i] = 1;
                visitedNum++;
                strcpy(queue[queueSize], edges[i][0]);
                queueSize++;
            }
        }
    }
}

int main(int argc, char **argv)
{
    struct addrinfo hints;
    struct sockaddr_storage clientaddr;
    struct addrinfo *res;
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    int portT;
    int rc, socketfd;
    char names[2][MAXLEN];
    socklen_t clientlen;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    // function from "Beej???s Guide to Network ProgrammingUsing Internet Sockets"
    if ((rc = getaddrinfo(LOCALHOST, UDPPORT, &hints, &res)) != 0)
    {
        fprintf(stderr, "getaddrinfo failed (port %s)\n", UDPPORT);
        return -2;
    }

    while (1)
    {
        // printf("Create a socket descriptor\n");
        /* Create a socket descriptor */
        // function from "Beej???s Guide to Network ProgrammingUsing Internet Sockets"
        if ((socketfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0)
        {
            fprintf(stderr, "socket() failed (port %s)\n", UDPPORT);
            return -2;
        }
        // printf("Bind the descriptor to the port\n");
        /* Bind the descriptor to the port */
        // function from "Beej???s Guide to Network ProgrammingUsing Internet Sockets"
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
            portT = ntohs(sin.sin_port);
        }
        printf("The serverT is up and running using UDP on port %d.\n", portT);
        clientlen = sizeof(clientaddr);

        // printf("recvfrom the client\n");
        int namesInx = 0;
        int num = 0;
        /*reveive names*/
        while (namesInx != 2)
        {
            char buf[MAXLEN] = "";
            // function from "Beej???s Guide to Network ProgrammingUsing Internet Sockets"
            if (num = recvfrom(socketfd, buf, MAXLEN - 1, 0, (struct sockaddr *)&clientaddr, &clientlen) < 0)
            {
                fprintf(stderr, "recvfrom() failed (port %s)\n", UDPPORT);
                return -2;
            }
            // printf("recerice length: %d\n", num);
            strcpy(names[namesInx], buf);
            // printf("name is: %s\n", names[namesInx]);
            namesInx++;
        }
        printf("The ServerT received a request from Central to get the topology.\n");
        // printf("find all edges\n");
        /*find all edges*/
        char edges[MAXLEN][2][MAXLEN];
        int visited[MAXLEN];
        memset(visited, 0, sizeof(visited));
        int edgeInx = findEdge(names, edges, visited);
        /*end find all edges*/
        /*send all edges back to central*/

        for (int i = 0; i < edgeInx; i++)
        {
            // printf("visited %d\n", visited[i]);
            if (visited[i])
            {
                for(int i = 0; i < 50000; i++){
                    continue;
                }
                // function from "Beej???s Guide to Network ProgrammingUsing Internet Sockets"
                if (sendto(socketfd, edges[i], sizeof(edges[i]), 0, (struct sockaddr *)&clientaddr, clientlen) < 0)
                {
                    fprintf(stderr, "sendto() failed (port %s)\n", UDPPORT);
                    return -2;
                }
                // printf("sent %s, %s\n", edges[i][0], edges[i][1]);
            }
        }
        char end[] = "end";
        // function from "Beej???s Guide to Network ProgrammingUsing Internet Sockets"
        if (sendto(socketfd, end, sizeof(end), 0, (struct sockaddr *)&clientaddr, clientlen) < 0)
        {
            fprintf(stderr, "sendto() failed (port %s)\n", UDPPORT);
            return -2;
        }
        /*end send all edges back to central*/
        printf("The ServerT finished sending the topology to Central.\n");
        close(socketfd);
    }
}