
#include <iostream>
#include <fstream>
#include <string>
#include <bits/stdc++.h>
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
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "udpClass.cpp"
using namespace std;
#define A_PORT 25592
#define B_PORT 26592
#define T_PORT 21592
#define S_PORT 22592
#define P_PORT 23592
#define C_PORT 24592
#define HOSTNAME "127.0.0.1"

// send topology information by UDP
void sendGraph(unordered_map<string, list<string>> adjacencyList, udp_socket_class server)
{
    server.sendData("start");
    for (auto &value : adjacencyList)
    {
        string vertex = value.first;
        list<string> adjacentVertices = value.second;
        list<string>::iterator itr = adjacentVertices.begin();

        server.sendData("vertex" + vertex);

        while (itr != adjacentVertices.end())
        {
            server.sendData("edge" + *itr);
            ++itr;
        }
    }
    server.sendData("end");
}

//implement BFS algorithm to find the subgraph containing both u and w
unordered_map<string, list<string>> BFS(string u, string w, unordered_map<string, list<string>> adj)
{
    unordered_map<string, bool> visited;
    unordered_map<string, list<string>> resGraph;

    list<string> q;

    visited[u] = true;
    q.push_back(u);
    bool hasW = false;
    while (!q.empty())
    {
        u = q.front();
        q.pop_front();

        list<string>::iterator itr = adj[u].begin();

        while (itr != adj[u].end())
        {
            if (!visited[*itr])
            {
                visited[*itr] = true;
                q.push_back(*itr);
            }
            resGraph[u].push_back(*itr);
            if (*itr == w) {
                hasW = true;
            }
            ++itr;
        }
    }
    if (hasW) {
        
        return resGraph;
    }
    
}

string trim(const string &s)
{
    auto start = s.begin();
    while (start != s.end() && isspace(*start))
    {
        start++;
    }

    auto end = s.end();
    do
    {
        end--;
    } while (distance(start, end) > 0 && isspace(*end));

    return string(start, end + 1);
}

// Driver code
int main()
{
    unordered_map<string, list<string>> adjacencyList;
    fstream newfile;

    newfile.open("edgelist.txt", ios::in); //open a file to perform read operation using file object
    if (newfile.is_open())
    { //checking whether the file is open
        string tp;
        while (getline(newfile, tp))
        { //read data from file object and put it into string.
            string firstName;
            string secondName;
            string name;
            for (auto c : tp)
            {
                if (c == ' ')
                {
                    firstName = name;
                    name = "";
                }
                else
                {
                    name = name + c;
                }
                secondName = name;
            }
            adjacencyList[firstName].push_back(secondName);
            adjacencyList[secondName].push_back(firstName);
        }
        newfile.close(); //close the file object.
    }


    while (true)
    {
        // -- phase 1A --
        udp_socket_class server(HOSTNAME, T_PORT, HOSTNAME, C_PORT);
        server.start();
        printf("The ServerT is up and running using UDP on port %d.\n", T_PORT);

        server.recvData();
        // -- phase 1B --
        if (server.recvString == "Request a connection") {
            server.sendData("OK");
        }else {
            cout << "Connection Error" << endl;
        }
        // -- phase 2 -- 
        server.recvData();
        string nameA = trim(server.recvString);
        server.recvData();
        string nameB = trim(server.recvString); 
        printf("The ServerT received a request from Central to get the topology.\n");
        // get subgraph
        unordered_map<string, list<string>> resGraph = BFS(nameA, nameB, adjacencyList);
        // send back
        sendGraph(resGraph, server);
        printf("The ServerT finished sending the topology to Central.\n");
        server.closeUdpSocket();
    }
    return 0;
}