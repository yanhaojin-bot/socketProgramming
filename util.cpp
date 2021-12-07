#include <string.h>
#include <string>
#include <bits/stdc++.h>
#include "udpClass.cpp"

using namespace std;

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

void receiveTopoInfo(udp_socket_class &server, unordered_map<string, list<string>> &adjacencyList)
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
                // cout << adjacencyList.size() << endl;
            }

            adjacentVertices = {};
            vertexName = server.recvString.substr(6);
        }
        else if (server.recvString.substr(0, 4) == "edge")
        {
            string edgeName = server.recvString.substr(4);
            adjacentVertices.emplace_back(edgeName);
        }
        server.recvData();
    }
}

void receiveScoresInfo(udp_socket_class &server, unordered_map<string, int> &scores, unordered_map<string, int> name2Index)
{
    int i = 0;
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
        name2Index[name] = i;
        i++;
        scores[name] = score;
        server.recvData();
    }
}

void sendResults(udp_socket_class &server, double minSum, vector<string> &minPath)
{
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
}
