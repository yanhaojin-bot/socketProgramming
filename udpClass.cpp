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
#include <unistd.h>
using namespace std;

// modify Datagram Sockets tutorial from Beej’s Guide to Network Programming
const string HOSTNAME = "localhost";

class udp_socket_class
{
private:
	void showErr(const char *);
	int bufferSize;
	int udpSocket;
	char localIP[32];
	socklen_t localPort;
	char peerIP[32];
	socklen_t peerPort;
	struct sockaddr_in peer_address;
	struct sockaddr_in local_address;
public:

	string recvString;
	udp_socket_class(string, int, string, int);
	void closeUdpSocket();
	int start();
	int sendData(string);
	int recvData();
	int update(sockaddr_in);
	socklen_t getPeerPort();
};

socklen_t udp_socket_class::getPeerPort() {
	return this->peerPort;
}

int udp_socket_class::recvData()
{
	socklen_t lengthOfAddress = sizeof(struct sockaddr);
	
	// receive the length of data
	int recvLength = 0;
	if (recvfrom(this->udpSocket, &recvLength, sizeof(recvLength), 0, (struct sockaddr *)&this->peer_address, &lengthOfAddress) < 0)
	{
		this->showErr("Receive Error");
		return -1;
	}
	recvLength = ntohl(recvLength);

	// get peer IP and port
	strcpy(this->peerIP, inet_ntoa(this->peer_address.sin_addr));
	this->peerPort = ntohs(this->peer_address.sin_port);

	// receive data
	char buffer[this->bufferSize];
	memset(buffer, 0, this->bufferSize);
	if ((recvfrom(this->udpSocket, buffer, recvLength, 0, (struct sockaddr *)&this->peer_address, &lengthOfAddress)) < 0)
	{
		cout << "receive error" << endl;
		this->showErr("Receive Error");
		return -1;
	}
	this->recvString = buffer;
	cout << "";
	sleep(0.001);
	return 1;
}

int udp_socket_class::sendData(string str)
{
	int sendLength = htonl(str.size());
	socklen_t lengthOfAddress = sizeof(struct sockaddr);

	// send the length of data
	if ((sendto(this->udpSocket, &sendLength, sizeof(sendLength), 0, (struct sockaddr *)&this->peer_address, lengthOfAddress)) < 0)
	{
		this->showErr("Send length error");
		return -1;
	}

	// send the data
	if ((sendto(this->udpSocket, str.c_str(), str.size(), 0, (struct sockaddr *)&this->peer_address, lengthOfAddress)) < 0)
	{
		this->showErr("Send data error");
		return -1;
	}
	cout << "";
	sleep(0.001);
	return 1;
}

int udp_socket_class::start()
{
	this->udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (udpSocket < 0)
	{
		showErr("Socket cannot create");
	}

	if (bind(this->udpSocket, (struct sockaddr *)&this->local_address, sizeof(struct sockaddr)) < 0)
	{
		showErr("Bind Error");
		return -1;
	}

	return 0;
}

int udp_socket_class::update(sockaddr_in peer){
	// update port number after udp socket been created
	this->peerPort = peer.sin_port;
	this->peer_address.sin_port = peer.sin_port;
}

udp_socket_class::udp_socket_class(string srcHost, int srcPort, string dstHost, int dstPort)
{
	this->bufferSize = 128;
	this->localPort = srcPort;
	this->peerPort = dstPort;

	// get the local IP address and port
	struct hostent *local;
	local = gethostbyname(srcHost.c_str());
	strcpy(this->localIP, inet_ntoa(*((struct in_addr *)local->h_addr)));

	memset((char *) &this->local_address, 0, sizeof(this->local_address));
	this->local_address.sin_family = AF_INET;
	this->local_address.sin_port = htons(srcPort);
	this->local_address.sin_addr = *((struct in_addr *)local->h_addr);

	// get the target IP address and port 
	struct hostent *peer;
	peer = gethostbyname(dstHost.c_str());
	strcpy(this->peerIP, inet_ntoa(*((struct in_addr *)peer->h_addr)));

	memset((char *) &this->peer_address, 0, sizeof(this->peer_address));
	this->peer_address.sin_family = AF_INET;
	this->peer_address.sin_port = htons(dstPort);
	this->peer_address.sin_addr = *((struct in_addr *)peer->h_addr);
}

void udp_socket_class::closeUdpSocket()
{
	close(this->udpSocket);
}

void udp_socket_class::showErr(const char *err)
{
	perror(err);
	exit(1);
}