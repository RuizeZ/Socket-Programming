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
#include <math.h>
#include <float.h>
#define UDPPORP "23069"
#define LOCALHOST "127.0.0.1"
#define MAXLEN 512

void dijkstra(char edges[][2][MAXLEN], int edgeInx, char scoresneeded[][2][MAXLEN], int scoresneededsize, char *inputname1, char *inputname2, char finalpath[][MAXLEN], double *returnArray);
int findnameindex(char scoresneeded[][2][MAXLEN], int scoresneededsize, char *name);

/*find the index of the name in scoresneeded to build the adjacency matrix*/
int findnameindex(char scoresneeded[][2][MAXLEN], int scoresneededsize, char *name)
{
    for (int i = 0; i < scoresneededsize; i++)
    {
        if (strcmp(scoresneeded[i][0], name) == 0)
        {
            return i;
        }
    }
    return -1;
}
/*end findnameindex*/

/*dijkstra algorithm */
void dijkstra(char edges[][2][MAXLEN], int edgeInx, char scoresneeded[][2][MAXLEN], int scoresneededsize, char *inputname1, char *inputname2, char finalpath[][MAXLEN], double *returnArray)
{
    char queue[edgeInx * 2][MAXLEN];
    int visited[scoresneededsize];
    memset(visited, 0, sizeof(visited));
    int queueSize = 0;
    strcpy(queue[0], inputname1);
    queueSize++;
    /*Create adjacency matrix adj[ ][ ]. If i and j are connected, adj[i][j] = adj[j][i] = 1, otherwise, 0*/
    int adj[scoresneededsize][scoresneededsize];
    /*set all to 0*/
    for (int i = 0; i < scoresneededsize; i++)
    {
        for (int j = 0; j < scoresneededsize; j++)
        {
            adj[i][j] = 0;
        }
    }
    /*end set all to 0*/
    /*If i and j are connected, adj[i][j] = adj[j][i] = 1, otherwise, 0*/
    for (int i = 0; i < edgeInx; i++)
    {
        char name1[MAXLEN], name2[MAXLEN];
        memset(name1, 0, sizeof(name1));
        memset(name2, 0, sizeof(name2));
        int name1index, name2index;
        strcpy(name1, edges[i][0]);
        name1index = findnameindex(scoresneeded, scoresneededsize, name1);
        strcpy(name2, edges[i][1]);
        name2index = findnameindex(scoresneeded, scoresneededsize, name2);
        if (name1index == -1 || name2index == -1)
        {
            // printf("name1: %s\n", name1);
            // printf("name2: %s\n", name2);
            // printf("name is not in the array\n");
            return;
        }
        // Update value to 1
        adj[name1index][name2index] = 1;
        adj[name2index][name1index] = 1;
    }
    /*end create adjacency matrix*/

    // for (int i = 0; i < scoresneededsize; i++)
    // {
    //     for (int j = 0; j < scoresneededsize; j++)
    //     {
    //         printf("%d ", adj[i][j]);
    //     }
    //     printf("\n");
    // }

    /*Create cost matrix cost[ ][ ] from adjacency matrix adj[ ][ ]. cost[i][j] is the cost of going from vertex i to vertex j.
    If there is no edge between vertices i and j then cost[i][j] is infinity.*/
    double cost[scoresneededsize][scoresneededsize];
    int startnode = -1;
    char startname[MAXLEN], endname[MAXLEN];
    memset(startname, 0, sizeof(startname));
    memset(endname, 0, sizeof(endname));
    for (int i = 0; i < scoresneededsize; i++)
    {
        if (strcmp(scoresneeded[i][0], inputname1) == 0 || strcmp(scoresneeded[i][0], inputname2) == 0)
        {
            if (startnode == -1)
            {
                startnode = i;
                strcpy(startname, scoresneeded[i][0]);
                if (strcmp(startname, inputname1) == 0)
                {
                    strcpy(endname, inputname2);
                }
                else
                {
                    strcpy(endname, inputname1);
                }
            }
        }
        for (int j = i; j < scoresneededsize; j++)
        {
            if (adj[i][j] == 1)
            {
                double number1 = strtod(scoresneeded[i][1], NULL);
                double number2 = strtod(scoresneeded[j][1], NULL);
                double distance = (fabs(number1 - number2)) / (fabs(number1 + number2));
                // only keep to hundredth without rounding, might overflow
                // distance = (int)(distance * 100) / 100.0;
                cost[i][j] = distance;
                cost[j][i] = distance;
            }
            else
            {
                cost[i][j] = -1;
                cost[j][i] = -1;
            }
        }
    }
    /*end cost[][]*/

    // for (int i = 0; i < scoresneededsize; i++)
    // {
    //     for (int j = 0; j < scoresneededsize; j++)
    //     {
    //         printf("%f ", cost[i][j]);
    //     }
    //     printf("\n");
    // }

    /*create distance[i], distance[i] means the smallest cost from startnode to i*/
    double distance[scoresneededsize];
    for (int i = 0; i < scoresneededsize; i++)
    {
        if (cost[startnode][i] != -1)
        {
            distance[i] = cost[startnode][i];
        }
        else
        {
            distance[i] = DBL_MAX;
        }
    }
    distance[startnode] = 0;
    /*end create distance[i]*/
    // for (int i = 0; i < scoresneededsize; i++)
    // {

    //     printf("%f ", distance[i]);
    // }
    // printf("\n");

    /*create prevnode[], prevneode[i] means the prevnode of ith node on the shortest path*/
    int prevnode[scoresneededsize];
    for (int i = 0; i < scoresneededsize; i++)
    {
        if (distance[i] != -1)
        {
            prevnode[i] = startnode;
        }
        else
        {
            prevnode[i] = -1;
        }
    }
    /*end create prevnode[]*/

    /*start Dijkstra*/
    int count = 0;
    visited[startnode] = 1;
    count++;
    while (count != scoresneededsize)
    {
        /*find min value index i in the distance[], change visited[i]*/
        double minValue = DBL_MAX;
        int minInx = 0;
        for (int i = 0; i < scoresneededsize; i++)
        {
            if (distance[i] > 0 && !visited[i])
            {
                if (distance[i] < minValue)
                {
                    minValue = distance[i];
                    minInx = i;
                }
            }
        }
        visited[minInx] = 1;
        count++;
        /*check if we find the endname*/
        if (strcmp(scoresneeded[minInx][0], endname) == 0)
        {
            // printf("find path\n");
            /*create finalPath*/
            int finalpathInx = 0;
            strcpy(finalpath[finalpathInx], endname);
            finalpathInx++;
            int nextnode = prevnode[minInx];
            while (nextnode != startnode)
            {
                strcpy(finalpath[finalpathInx], scoresneeded[nextnode][0]);
                finalpathInx++;
                nextnode = prevnode[nextnode];
            }
            strcpy(finalpath[finalpathInx], startname);
            finalpathInx++;
            returnArray[0] = finalpathInx;
            returnArray[1] = minValue;
            return;
        }
        /*end find min value index*/

        /*update the distance[i] if a less distence is found from minInx to i*/
        for (int i = 0; i < scoresneededsize; i++)
        {
            if (!visited[i])
            {
                if (cost[minInx][i] != -1)
                {
                    if (cost[minInx][i] + minValue < distance[i])
                    {
                        distance[i] = cost[minInx][i] + minValue;
                        prevnode[i] = minInx;
                    }
                }
            }
        }
        /*end update the distance[i]*/
    }
    // printf("no path found\n");
}

int main(int argc, char **argv)
{
    struct addrinfo hints;
    struct sockaddr_storage clientaddr;
    struct addrinfo *res;
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    int portP;
    int rc, socketfd;
    socklen_t clientlen;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    // function from "Beej’s Guide to Network ProgrammingUsing Internet Sockets"
    if ((rc = getaddrinfo(LOCALHOST, UDPPORP, &hints, &res)) != 0)
    {
        fprintf(stderr, "getaddrinfo failed (port %s)\n", UDPPORP);
        return -2;
    }
    while (1)
    {
        /* Create a socket descriptor */
        // function from "Beej’s Guide to Network ProgrammingUsing Internet Sockets"
        if ((socketfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0)
        {
            fprintf(stderr, "socket() failed (port %s)\n", UDPPORP);
            return -2;
        }
        /* Bind the descriptor to the port */
        // function from "Beej’s Guide to Network ProgrammingUsing Internet Sockets"
        if (bind(socketfd, res->ai_addr, res->ai_addrlen) < 0)
        {
            fprintf(stderr, "bind() failed (port %s)\n", UDPPORP);
            return -2;
        }
        if (getsockname(socketfd, (struct sockaddr *)&sin, &len) == -1)
        {
            perror("getsockname");
        }
        else
        {
            portP = ntohs(sin.sin_port);
        }
        printf("The serverP is up and running using UDP on port %d.\n", portP);
        /*receive edges from client*/
        clientlen = sizeof(clientaddr);
        char edges[MAXLEN][2][MAXLEN];
        int edgeInx = 0;
        while (1)
        {
            // function from "Beej’s Guide to Network ProgrammingUsing Internet Sockets"
            if (recvfrom(socketfd, edges[edgeInx], sizeof(edges[edgeInx]), 0, (struct sockaddr *)&clientaddr, &clientlen) < 0)
            {
                fprintf(stderr, "recvfrom() failed (port %d)\n", portP);
                return -2;
            }
            if (strcmp(edges[edgeInx][0], "end") == 0)
            {
                break;
            }
            // printf("receive %s, %s\n", edges[edgeInx][0], edges[edgeInx][1]);
            edgeInx++;
        }
        /*end receive edges*/

        /*receive scores from client*/
        char scoresneeded[MAXLEN][2][MAXLEN];
        int scoresneededsize = 0;
        while (1)
        {
            // function from "Beej’s Guide to Network ProgrammingUsing Internet Sockets"
            if (recvfrom(socketfd, scoresneeded[scoresneededsize], sizeof(scoresneeded[scoresneededsize]), 0, (struct sockaddr *)&clientaddr, &clientlen) < 0)
            {
                fprintf(stderr, "recvfrom() failed (port %d)\n", portP);
                return -2;
            }
            if (strcmp(scoresneeded[scoresneededsize][0], "end") == 0)
            {
                break;
            }
            // printf("receive %s, %s\n", scoresneeded[scoresneededsize][0], scoresneeded[scoresneededsize][1]);
            scoresneededsize++;
        }
        /*end receive scores from client*/

        /*receive input names from client*/
        char inputname1[MAXLEN] = "";
        char inputname2[MAXLEN] = "";
        int num;
        // printf("receive name1 %s\n", inputname1);
        // function from "Beej’s Guide to Network ProgrammingUsing Internet Sockets"
        if (num = recvfrom(socketfd, inputname1, MAXLEN - 1, 0, (struct sockaddr *)&clientaddr, &clientlen) < 0)
        {
            fprintf(stderr, "recvfrom() failed (port %d)\n", portP);
            return -2;
        }
        // printf("recerice length: %d\n", num);
        // printf("receive name1 %s\n", inputname1);
        // function from "Beej’s Guide to Network ProgrammingUsing Internet Sockets"
        if (recvfrom(socketfd, inputname2, MAXLEN - 1, 0, (struct sockaddr *)&clientaddr, &clientlen) < 0)
        {
            fprintf(stderr, "recvfrom() failed (port %d)\n", portP);
            return -2;
        }
        // printf("receive name2 %s\n", inputname2);
        /*end receive input names from client*/
        printf("The ServerP received the topology and score information.\n");
        char finalpath[MAXLEN][MAXLEN];
        memset(finalpath, 0, sizeof(finalpath));
        double returnArray[2];
        memset(returnArray, 0, sizeof(returnArray));
        int finalpathInx;
        // for (int i = 0; i < edgeInx; i++)
        // {
        //     printf("%s, %s\n", edges[i][0], edges[i][1]);
        // }
        // for (int i = 0; i < scoresneededsize; i++)
        // {
        //     printf("%s, %s\n", scoresneeded[i][0], scoresneeded[i][1]);
        // }
        dijkstra(edges, edgeInx, scoresneeded, scoresneededsize, inputname1, inputname2, finalpath, returnArray);
        finalpathInx = returnArray[0];
        // for (int i = 0; i < finalpathInx; i++)
        // {
        //     printf("%s\n", finalpath[i]);
        // }
        // printf("min cost = %.2f\n", minValue);
        // inet_ntop(res->ai_family, (struct sockaddr *)&clientaddr, ipstr, sizeof(ipstr));
        // printf("Get connected with %s\n", ipstr);

        /*send all edges back to central*/
        for (int i = 0; i < finalpathInx; i++)
        {
            for(int i = 0; i < 50000; i++){
                continue;
            }
            // function from "Beej’s Guide to Network ProgrammingUsing Internet Sockets"
            if (sendto(socketfd, finalpath[i], sizeof(finalpath[i]), 0, (struct sockaddr *)&clientaddr, clientlen) < 0)
            {
                fprintf(stderr, "sendto() failed (port %d)\n", portP);
                return -2;
            }
            // printf("sent %s\n", finalpath[i]);
        }
        char end[] = "end";
        // function from "Beej’s Guide to Network ProgrammingUsing Internet Sockets"
        if (sendto(socketfd, end, sizeof(end), 0, (struct sockaddr *)&clientaddr, clientlen) < 0)
        {
            fprintf(stderr, "sendto() failed (port %d)\n", portP);
            return -2;
        }
        // function from "Beej’s Guide to Network ProgrammingUsing Internet Sockets"
        if (sendto(socketfd, returnArray + 1, sizeof(returnArray + 1), 0, (struct sockaddr *)&clientaddr, clientlen) < 0)
        {
            fprintf(stderr, "sendto() failed (port %d)\n", portP);
            return -2;
        }
        printf("The ServerP finished sending the results to the Central.\n");
        // printf("sent %.2f\n", *(returnArray + 1));
        /*end send all edges back to central*/
        close(socketfd);
    }
}