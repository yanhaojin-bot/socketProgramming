
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>
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
#include <vector>
#include "udpClass.cpp"
#include "util.cpp"
#define A_PORT 25592
#define B_PORT 26592
#define T_PORT 21592
#define S_PORT 22592
#define P_PORT 23592
#define C_PORT 24592
#define HOSTNAME "127.0.0.1"
using namespace std;

unordered_map<string, list<string> > adjacencyList;
unordered_map<string, int> scores;

double minSum;
vector<string> minPath;

// calculate matching gap
double getMatchingGap(string name1, string name2)
{
    int s1 = scores[name1];
    int s2 = scores[name2];

    if (s1 == 0 || s2 == 0)
    {
        return 0;
    }

    return (double)(abs(s1 - s2)) / (double)(s1 + s2);
}

// implement DFS algorithm to find all possible paths from s to d
// citation: https://www.geeksforgeeks.org/count-possible-paths-two-vertices/
void findAllPathsUtils(string s, string d, unordered_map<string, bool> &visited, vector<string> &path)
{
    if (s == d)
    {
        double sumGap = 0;
        for (int i = 0; i < path.size() - 1; i++)
        {
            sumGap += getMatchingGap(path[i], path[i + 1]);
        }

        if (sumGap < minSum)
        {
            minPath = path;
            minSum = sumGap;
        }
        return;
    }
    visited[s] = true;
    list<string> neighbors = adjacencyList[s];
    for (string neighbor : neighbors)
    {
        if (visited[neighbor] == false)
        {
            path.push_back(neighbor);
            findAllPathsUtils(neighbor, d, visited, path);
            path.pop_back();
        }
    }
}

// DFS driver function
void findAllPaths(string s, string d)
{
    int size = adjacencyList.size();
    unordered_map<string, bool> visited(size);

    for (auto &pair : adjacencyList)
    {
        visited[pair.first] = false;
    }
    vector<string> path;
    path.push_back(s);
    findAllPathsUtils(s, d, visited, path);
}

int main(int argc, char const *argv[])
{

    // loop
    while (true)
    {
        // -- phase 1A --
        udp_socket_class server(HOSTNAME, P_PORT, HOSTNAME, C_PORT);
        server.start();
        printf("The ServerP is up and running using UDP on port %d.\n", P_PORT);
        minSum = DBL_MAX;
        minPath.clear();
        adjacencyList.clear();
        // -- phase 1B --
        server.recvData();
        if (server.recvString == "Request a connection")
        {
            server.sendData("OK");
        }
        else
        {
            cout << "Connection Error" << endl;
        }
        // -- phase 2 --
        // receive topology information
        server.recvData();
        if (server.recvString == "topology")
        {
            string vertexName = "";
            list<string> adjacentVertices;
            while (server.recvString != "end")
            {
                if (server.recvString.substr(0, 6) == "vertex")
                {

                    if (vertexName != "")
                    {
                        adjacencyList[vertexName] = adjacentVertices;
                        //cout << adjacencyList.size() << endl;
                    }

                    adjacentVertices = {};
                    vertexName = server.recvString.substr(6);
                }
                else if (server.recvString.substr(0, 4) == "edge")
                {
                    string edgeName = server.recvString.substr(4);
                    adjacentVertices.push_back(edgeName);
                }
                server.recvData();
            }
        }

        // receive score information
        server.recvData();
        if (server.recvString == "scores")
        {
            server.recvData();
            while (server.recvString != "end")
            {
                string name;
                string scoreStr;
                string message = server.recvString;
                for (int i = 0; i < message.size(); i++)
                {
                    if (message.at(i) == '%')
                    {
                        name = message.substr(0, i);
                        scoreStr = message.substr(i + 1);
                        break;
                    }
                }

                int score = stoi(scoreStr, nullptr, 10);
                scores[name] = score;
                server.recvData();

            }
        }
        cout << "The ServerP received the topology and score information." << endl;

        // receive names inquired from serverC
        server.recvData();
        string nameA = trim(server.recvString);
        server.recvData();
        string nameB = trim(server.recvString);

        findAllPaths(nameA, nameB);
        server.sendData("sum");
        if (minSum == DBL_MAX)
        {
            minSum = 0.0;
        }
        stringstream stream;
        stream << fixed << setprecision(2) << minSum;
        string value = stream.str();

        // send matching gap
        server.sendData(value);

        // send the path
        server.sendData("path");
        for (int i = 0; i < minPath.size(); i++)
        {
            server.sendData(minPath[i]);
        }

        server.sendData("end");
        cout << "The ServerP finished sending the results to the Central." << endl;
        server.closeUdpSocket();
    }

    return 0;
}
