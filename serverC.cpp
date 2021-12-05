// Server side C/C++ program to demonstrate Socket programming
// ID: 2266578592
// TCP Port with client A: 25592
// TCP Port with client B: 26592

#include <iostream>
#include <fstream>
#include <string>
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
#include "tcp_server.cpp"
#include <bits/stdc++.h>
#include <unistd.h>
#define A_PORT 25592
#define B_PORT 26592
#define T_PORT 21592
#define S_PORT 22592
#define P_PORT 23592
#define C_PORT 24592
#define HOSTNAME "127.0.0.1"

using namespace std;
// send topological information to serverP
void sendGraph(unordered_map<string, list<string> > &adjacencyList, udp_socket_class client)
{
	client.sendData("start");
	for (auto &value : adjacencyList)
	{
		string vertex = value.first;
		list<string> adjacentVertices = value.second;
		list<string>::iterator itr = adjacentVertices.begin();

		client.sendData("vertex" + vertex);
		//server.sendData();

		while (itr != adjacentVertices.end())
		{
			client.sendData("edge" + *itr);
			//server.sendData();
			++itr;
		}
	}
	client.sendData("end");
}

// send score information to serverP
void sendTable(unordered_map<string, int> table, int V, udp_socket_class client)
{
	for (auto &value : table)
	{
		string name = value.first;
		int score = value.second;
		string sendMessage = name;
		sendMessage.append("%");
		sendMessage.append(to_string(score));
		client.sendData(sendMessage);
	}
	client.sendData("end");
}

int main(int argc, char const *argv[])
{
	printf("The Central server is up and running.\n");

	// udp client instance
	udp_socket_class udpClient(HOSTNAME, C_PORT, HOSTNAME, T_PORT);

	// tcp server to communicate with clientA
	tcp_server serverA = tcp_server(A_PORT);
	// tcp server to communicate with clientB
	tcp_server serverB = tcp_server(B_PORT);
	while (true)
	{
		// ---- Phase 1A ----

		serverA.accpet();
		// listen to client A
		serverA.recvData();
		printf("The Central server received input= \"%s\" from the client using TCP over port %d.\n", serverA.recvString.c_str(), A_PORT);
		string nameA = serverA.recvString;

		// listen to client B:
		serverB.accpet();
		serverB.recvData();
		string clientSize = serverB.recvString;
		bool isTwoClient = false;
		// bonus part
		if (clientSize == "2")
		{
			isTwoClient = true;
		}
		serverB.recvData();
		string nameB = serverB.recvString;
		string nameB2;
		if (isTwoClient)
		{
			serverB.recvData();
			nameB2 = serverB.recvString;
			printf("The Central server received input= \"%s\" and \"%s\" from the client using TCP over port %d.\n", nameB.c_str(), nameB2.c_str(), B_PORT);
		}
		else
		{
			printf("The Central server received input=\"%s\" from the client using TCP over port %d.\n", nameB.c_str(), B_PORT);
		}

		// ---- Phase 1B ----
		int loopTimes;
		if (isTwoClient)
		{
			loopTimes = 2;
		}
		else
		{
			loopTimes = 1;
		}
		while (loopTimes > 0)
		{
			udpClient.start();
			// try to connect UDP servers cluster
			struct sockaddr_in peerT;
			peerT.sin_port = htons(T_PORT);
			udpClient.update(peerT);
			udpClient.sendData("Request a connection");
			udpClient.recvData();
			if (udpClient.recvString != "OK")
			{
				cout << "T connection error at" << udpClient.getPeerPort() << endl;
			}
			else
			{
				//cout << "T  ok" << endl;
			}

			struct sockaddr_in peerS;
			peerS.sin_port = htons(S_PORT);
			udpClient.update(peerS);
			udpClient.sendData("Request a connection");
			udpClient.recvData();
			if (udpClient.recvString != "OK")
			{
				cout << "S connection error at " << udpClient.getPeerPort() << endl;
			}
			else
			{
				//cout << "S  ok" << endl;
			}

			struct sockaddr_in peerP;
			peerP.sin_port = htons(P_PORT);
			udpClient.update(peerP);
			udpClient.sendData("Request a connection");
			udpClient.recvData();
			if (udpClient.recvString != "OK")
			{
				cout << "P connection error at " << udpClient.getPeerPort() << endl;
			}
			else
			{
				//cout << "P  ok" << endl;
			}

			// ---- Phase 2 ----
			// send to T
			if (isTwoClient == true && loopTimes == 1)
			{
				nameB = nameB2;
			}
			udpClient.update(peerT);
			udpClient.sendData(nameA);
			udpClient.sendData(nameB);
			cout << "The Central server sent a request to Backend-Server T" << endl;

			// receive from T
			unordered_map<string, list<string> > adjacencyList; // return graph
			udpClient.recvData();
			set<string> candidates; //all vertices in the return graph
			if (udpClient.recvString == "start")
			{
				list<string> adjacentVertices;
				string vertexName = "";
				while (udpClient.recvString != "end")
				{
					if (udpClient.recvString.substr(0, 6) == "vertex")
					{

						if (vertexName != "")
						{
							adjacencyList[vertexName] = adjacentVertices;
						}

						adjacentVertices = {};
						vertexName = udpClient.recvString.substr(6);
						candidates.insert(vertexName);
					}
					else if (udpClient.recvString.substr(0, 4) == "edge")
					{
						string edgeName = udpClient.recvString.substr(4);
						candidates.insert(edgeName);
						adjacentVertices.push_back(edgeName);
					}
					udpClient.recvData();
				}
				if (vertexName != "")
				{
					adjacencyList[vertexName] = adjacentVertices;
				}
				//cout << adjacencyList.size() << endl;
				printf("The Central server received information from Backend-Server T using UDP over port %d.\n", C_PORT);

				// send to S
				udpClient.update(peerS);
				set<string>::iterator it = candidates.begin();
				udpClient.sendData("start");
				// look up scores in the graph
				while (it != candidates.end())
				{
					udpClient.sendData(*it);
					it++;
				}
				udpClient.sendData("end");
				cout << "The Central server sent a request to Backend-Server S" << endl;

				// receive from S
				unordered_map<string, int> scores; // name : scores
				udpClient.recvData();
				//cout << candidates.size() << endl;
				if (udpClient.recvString == "start")
				{
					udpClient.recvData();
					while (udpClient.recvString != "end")
					{
						string name;
						string scoreStr;
						string message = udpClient.recvString;
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
						udpClient.recvData();
					}
				}
				for (pair<string, int> element : scores)
				{
					string name = element.first;
					int score = element.second;
				}

				printf("The Central server received information from Backend-Server S using UDP over port %d \n", C_PORT);

				// send to P
				udpClient.update(peerP);
				udpClient.sendData("topology");
				sendGraph(adjacencyList, udpClient);
				udpClient.sendData("scores");
				sendTable(scores, scores.size(), udpClient);
				udpClient.sendData(nameA);
				udpClient.sendData(nameB);
				cout << "The Central server sent a processing request to Backend-Server P." << endl;
				string matchingGap;
				udpClient.recvData();
				if (udpClient.recvString == "sum")
				{
					udpClient.recvData();
					matchingGap = udpClient.recvString;
				}

				udpClient.recvData();
				vector<string> path;
				if (udpClient.recvString == "path")
				{

					udpClient.recvData();
					while (udpClient.recvString != "end")
					{
						path.push_back(udpClient.recvString);
						udpClient.recvData();
					}
				}

				udpClient.closeUdpSocket();

				cout << "The Central server received the results from backend server P." << endl;

				// -- Phase 3 ---
				bool hasCompatibility = false;
				if (path.size() > 1)
				{
					hasCompatibility = true;
				}

				if (hasCompatibility)
				{
					// send result to client A
					string returnTitle = "Found compatibility for " + nameA + " and " + nameB + ":\n";
					serverA.sendData(returnTitle);
					string returnPath = "";

					for (int i = 0; i < path.size(); i++)
					{

						if (i < path.size() - 1)
						{
							returnPath.append(path[i]);
							returnPath.append("---");
						}
						else
						{
							returnPath.append(path[i]);
							returnPath.append("\n");
						}
					}
					serverA.sendData(returnPath);
					string returnScore = "Matching Gap: ";
					returnScore.append(matchingGap);
					returnScore.append("\n");

					serverA.sendData(returnScore);
					string end = "end\n";
					if (!(isTwoClient == true && loopTimes == 2))
					{
						serverA.sendData(end);
					}

					// send result to client B
					returnTitle = "Found compatibility for " + nameB + " and " + nameA + ":\n";
					serverB.sendData(returnTitle);
					returnPath = "";
					for (int i = path.size() - 1; i >= 0; i--)
					{
						if (i > 0)
						{
							returnPath.append(path[i]);
							returnPath.append("---");
						}
						else
						{
							returnPath.append(path[i]);
							returnPath.append("\n");
						}
					}
					serverB.sendData(returnPath);
					serverB.sendData(returnScore);
					if (!(isTwoClient == true && loopTimes == 2))
					{
						serverB.sendData(end);
					}
				}
				else
				{
					// no compatibility
					string returnTitle = "Found no compatibility between " + nameA + " and " + nameB + ".\n";
					serverA.sendData(returnTitle);
					returnTitle = "Found no compatibility between " + nameB + " and " + nameA + ".\n";
					serverB.sendData(returnTitle);
					if (!(isTwoClient == true && loopTimes == 2))
					{
						string end = "end\n";
						serverA.sendData(end);
						serverB.sendData(end);
					}
				}
			}
			cout << "The Central server sent the results to client A." << endl;
			cout << "The Central server sent the results to client B." << endl;
			loopTimes--;
			sleep(0.1);
		}
	}

	return 0;
}