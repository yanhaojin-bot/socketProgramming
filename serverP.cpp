
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

// implement DFS algorithm to find all possible paths from s to d and update the path with minimum matching gap
void findAllPathsUtils(string s, string d, unordered_map<string, bool> &visited, vector<string> &path)
{
    if (s == d) {
        double sumGap = 0;
        for (int i = 0; i < path.size() - 1; i++) {
            sumGap += getMatchingGap(path[i], path[i + 1]);
        }
        if (sumGap < minSum) {
            minPath = path;
            minSum = sumGap;
        }
        return;
    }
    visited[s] = true;
    list<string> neighbors = adjacencyList[s];
    for (string neighbor : neighbors) {
        if (visited[neighbor] == false) {
            path.emplace_back(neighbor);
            findAllPathsUtils(neighbor, d, visited, path);
            path.pop_back();
        }
    }
}

// DFS driver function
void findAllPaths(string s, string d) {
    int size = adjacencyList.size();
    unordered_map<string, bool> visited(size);

    for (auto &pair : adjacencyList){
        visited[pair.first] = false;
    }
    vector<string> path;
    path.emplace_back(s);
    findAllPathsUtils(s, d, visited, path);
}

int main(int argc, char const *argv[]) {
    while (true) {
        udp_socket_class server(HOSTNAME, P_PORT, HOSTNAME, C_PORT);
        server.start();
        printf("The ServerP is up and running using UDP on port %d.\n", P_PORT);
        minSum = DBL_MAX;
        minPath.clear();
        adjacencyList.clear();
        server.recvData();
        if (server.recvString == "Request a connection") {
            server.sendData("OK");

        } else {
            cout << "Connection Error" << endl;
        }
        // receive topology information
        server.recvData();
        if (server.recvString == "topology") {
            receiveTopoInfo(server, adjacencyList);
        }

        // receive score information
        server.recvData();
        if (server.recvString == "scores") {
            receiveScoresInfo(server, scores);
        }
        cout << "The ServerP received the topology and score information." << endl;

        // receive names inquired from serverC
        server.recvData();
        string nameA = trim(server.recvString);
        server.recvData();
        string nameB = trim(server.recvString);

        findAllPaths(nameA, nameB);
        sendResults(server, minSum, minPath);
        cout << "The ServerP finished sending the results to the Central." << endl;
        server.closeUdpSocket();
    }
    return 0;
}