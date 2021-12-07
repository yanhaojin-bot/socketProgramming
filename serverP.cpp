#include <bits/stdc++.h>
#include "util.cpp"
#define P_PORT 23592
#define C_PORT 24592
#define HOSTNAME "127.0.0.1"
using namespace std;

unordered_map<string, list<string> > adjacencyList;
unordered_map<string, int> scores;
unordered_map<string, int> name2Index;
vector<vector<double> > scoresMatrix;
double min_sum;
vector<string> min_path;

// calculate matching gap
double getMatchingGap(string name1, string name2){
    int index1 = name2Index[name1];
    int index2 = name2Index[name2];
    if (scoresMatrix[index1][index2] != 0) {
        return scoresMatrix[index1][index2] ;
    }
    int s1 = scores[name1];
    int s2 = scores[name2];

    if (s1 == 0 || s2 == 0) {
        return 0;
    }

    double matchingGap =  (double)(abs(s1 - s2)) / (double)(s1 + s2);
    scoresMatrix[index1][index2] = matchingGap;
    scoresMatrix[index2][index1] = matchingGap;
    return matchingGap; 
}

// implement DFS algorithm to find all possible paths from s to d and update the path with minimum matching gap
void findAllPathsUtils(string s, string d, unordered_map<string, bool> visited, vector<string> path, double curr_sum)
{
    if (curr_sum > min_sum) { return; } //decision tree pruning
    if (s == d) {
        double sumGap = curr_sum;
        min_path = path;
        min_sum = sumGap;
        return;
    }
    visited[s] = true;
    list<string> neighbors = adjacencyList[s];
    for (string neighbor : neighbors) {
        if (visited[neighbor] == false) {
            double mathching_gap = getMatchingGap(s, neighbor);
            double next_sum = curr_sum + mathching_gap;
            path.emplace_back(neighbor);
            findAllPathsUtils(neighbor, d, visited, path, next_sum);
            path.pop_back();
        }
    }
    visited[s] = false;
}

// DFS driver function
void findAllPaths(string s, string d) {
    int size = adjacencyList.size();
    unordered_map<string, bool> visited(size);
    for (auto &pair : adjacencyList) {
        visited[pair.first] = false;
    }
    vector<string> path;
    path.emplace_back(s);
    findAllPathsUtils(s, d, visited, path, 0);
}

int main(int argc, char const *argv[]) {
    while (true) {
        udp_socket_class server(HOSTNAME, P_PORT, HOSTNAME, C_PORT);
        server.start();
        printf("The ServerP is up and running using UDP on port %d.\n", P_PORT);
        min_sum = DBL_MAX;
        min_path.clear();
        adjacencyList.clear();
        name2Index.clear();
        scoresMatrix.clear();
        server.recvData();
        if (server.recvString == "Request a connection") {
            server.sendData("OK");
        } else {
            cout << "Connection Error" << endl;
        }
        // receive topology information
        server.recvData();
        if (server.recvString == "topology") { receiveTopoInfo(server, adjacencyList); }
        // receive score information
        server.recvData();
        if (server.recvString == "scores") { receiveScoresInfo(server, scores, name2Index); }
        int user_size = scores.size();
        vector<vector<double>> memo(user_size,vector<double>(user_size, 0));
        scoresMatrix = memo;
        cout << "The ServerP received the topology and score information." << endl;

        // receive names inquired from serverC
        server.recvData();
        string nameA = trim(server.recvString);
        server.recvData();
        string nameB = trim(server.recvString);

        findAllPaths(nameA, nameB);
        sendResults(server, min_sum, min_path);
        cout << "The ServerP finished sending the results to the Central." << endl;
        server.closeUdpSocket();
    }
    return 0;
}