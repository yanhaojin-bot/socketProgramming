#Socket Programming

## Functions implemented
- <b>Phase 1A</b>: boot up the cluster of servers, read the database file and establish the connections between the Clients and Server C
- <b>Phase 1B</b>: establish the connections between Server C and all other backend servers
- <b>Phase 2</b>: server C contacts the cluster of servers to retrieve information
- <b>Phase 3</b>: server C send back to the two clients 
- <b>Phase 4</b>: server C parses the message from client B with two usernames and retrieve the matching results

## Code files
- <b>clientA.cpp</b>: run it with one username can start clientA and connect to server C, receive the matching results from server C
- <b>clientB.cpp</b>: run it with one or two username(s) can start clientB and connect to server C, receive the matching results from server C separately
- <b>serverC.cpp</b>: run it can start serverC which is waiting for the request from clientA and clientB, try to connect with the cluster of servers
- <b>serverT.cpp</b>: run it can start serverT with waiting for the request from server C and reading the edges database file 
- <b>serverS.cpp</b>: run it can start serverS with waiting for the request from server C and reading the scores database file
- <b>serverP.cpp</b>: run it can start serverS with processing the information from server C
- <b>tcp_client.cpp</b>: the general tcp client class, both clientA and clientB have an instance of tcp_client
- <b>tcp_server.cpp</b>: the general tcp server class, server C have two instances of tcp_server to handle the connections with two clients
- <b>udpClass.cpp</b>: the general udp class, each client or server with UDP enabled has an instance of udpClass
- <b>util.cpp</b>: some tool functions, such as trim of string

## Messaged format
- <b>TCP</b>: <i>Length of data</i> data
- <b>UDP</b>: <i>Length of data</i> data
    - Topological information: start -> vertexName -> edgeName -> ...-> end
    - Scores information: start-> name%score -> ... -> end

## Idiosyncrasy
 - We have to start the processes in this order: Server C, Server T, Server S, Server P, Client A and Client B.


## Reused Code
 - Beej’s Guide to Network Programming 6.3 Datagram socket 
    - implement udpClass, including start(), sendData(), recvData(), update()
 - https://www.geeksforgeeks.org/count-possible-paths-two-vertices/ 
    -  implement DFS algorithm to find all possible paths from s to d
 - https://www.binarytides.com/code-a-simple-socket-client-class-in-c/
    - design tcp_client and tcp_server class

- https://stackoverflow.com/questions/216823/how-to-trim-a-stdstring
    - how to trim a string in c++