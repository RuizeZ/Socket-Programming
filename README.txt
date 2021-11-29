Name: Ruize Zhang
Student ID: 5423040069
In this project, I have done phases from Phase1 to Phase3 without Phase4.
clientA: send name to central and wait for result sent back from central
clientB: send name to central and wait for result sent back from central
central: receive two names from clientA and clientB, ask serverT for all relatived edges, ask serverS for the scores of each name and ask serverP 
         for the shortest path and smallest score. After all those, central sends the result to both clientA and clientB.
serverT: After receive names from central, serverT sends all relatived edges back to central
serverS: After receive edges from central, serverS sends scores for each name back to central
serverP: After receive edges and scores from central, serverP finds the shortest path, calculates the smallest score and sends those informations 
         back to central
All messages are exchanged in the format of multi-dimentional char array using either UDP or TCP
I used some of the code from book "Beej’s Guide to Network ProgrammingUsing Internet Sockets".
reused function: getaddrinfo(), socket(), bind(), sendto(), recvfrom(), listen(), accept(), recv(), send()