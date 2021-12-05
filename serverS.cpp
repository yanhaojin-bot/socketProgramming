
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
#define A_PORT 25592
#define B_PORT 26592
#define T_PORT 21592
#define S_PORT 22592
#define P_PORT 23592
#define C_PORT 24592
#define HOSTNAME "127.0.0.1"
using namespace std;

int main(int argc, char const *argv[])
{

    fstream newfile;
    int V = 6;
    unordered_map<string, int> scores(V + 1);
    newfile.open("scores.txt", ios::in); //open a file to perform read operation using file object
    if (newfile.is_open())
    { //checking whether the file is open
        string tp;
        while (getline(newfile, tp))
        { //read data from file object and put it into string.
            string name;
            string score;
            string word;
            for (auto c : tp)
            {
                if (c == ' ')
                {
                    name = word;
                    word = "";
                }
                else
                {
                    word = word + c;
                }
                score = word;
            }
            scores[name] = stoi(score, nullptr, 10);
        }
        newfile.close();
    }

    
    while (true)
    { // -- phase 1A --
        udp_socket_class server(HOSTNAME, S_PORT, HOSTNAME, C_PORT);
        server.start();    
        printf("The ServerS is up and running using UDP on port %d.\n", S_PORT);
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
        server.recvData();
        printf("The ServerS received a request from Central to get the scores. \n");
        unordered_map<string, int> candidateScores;
        if (server.recvString == "start")
        {
            server.recvData();
            while (server.recvString != "end")
            {
                string candidate = server.recvString;
                int score = scores[candidate];
                candidateScores[candidate] = score;
                server.recvData();
            }
        }

        // send back
        server.sendData("start");
        for (pair<string, int> element : candidateScores)
        {
            string name = element.first;
            int score = element.second;
            string sendMessage = name;
            sendMessage.append("%");
            sendMessage.append(to_string(score));
            server.sendData(sendMessage);
        }
        server.sendData("end");
        cout << "The ServerS finished sending the scores to Central." << endl;
        server.closeUdpSocket();
    }

    

    return 0;
}
