
#ifndef __HEADER_H_DEFINE__
#define __HEADER_H_DEFINE__

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
#include <vector>
#include <string>

#endif

using namespace std;
// citation: https://www.binarytides.com/code-a-simple-socket-client-class-in-c/
class tcp_client
{
private:
	int clientSocket;
	int portNum;
	struct hostent *hostInfo;
	struct sockaddr_in server_address; 
	void showErr(const char *err);
	int bufferSize;

public:
	tcp_client(string, int);
	int start();
	int sendData(string);
	int recvData();
	void closeClientSocket();

	string recvString;
};

tcp_client::tcp_client(string host, int port)
{
	this->portNum = port;
	this->clientSocket = -1;
	this->bufferSize = 1024;

	this->clientSocket = socket(AF_INET, SOCK_STREAM, 0);

	this->hostInfo = gethostbyname(host.c_str());

	memset((char *)&this->server_address, 0, sizeof(this->server_address));

	this->server_address.sin_family = AF_INET;
	memcpy(&this->server_address.sin_addr, this->hostInfo->h_addr, this->hostInfo->h_length);
	this->server_address.sin_port = htons(this->portNum);
}

int tcp_client::start()
{
	//  bound to your IP address and a random local port automatically.
	if (connect(this->clientSocket, (struct sockaddr *)&this->server_address, sizeof(this->server_address)) < 0)
	{
		return -1;
	}
	// get IP address and port number of client itself
	struct sockaddr_in myInfo;
	socklen_t myInfoLength = sizeof(myInfo);
	getsockname(this->clientSocket, (struct sockaddr *)&myInfo, &myInfoLength);
	return 1;
}

int tcp_client::recvData()
{
	int n;
	// receive length of data
	int recvLength = 0;
	if (read(this->clientSocket, &recvLength, sizeof(recvLength)) < 0)
	{
		showErr("Read Error");
		return -1;
	}

	// receive data
	char buffer[this->bufferSize];
	memset(buffer, 0, this->bufferSize);
	if ((read(this->clientSocket, buffer, recvLength) < 0))
	{
		showErr("Read Error");
		return -1;
	}
	this->recvString = buffer;
	return 1;
}

int tcp_client::sendData(string str)
{
	int sendLength = htonl(str.size());

	// send length of data 
	if ((write(this->clientSocket, &sendLength, sizeof(sendLength))) < 0)
	{
		this->showErr("Send Error");
		return -1;
	}

	// send data
	int n = 0;	
	if ((n = write(this->clientSocket, str.c_str(), str.size())) < 0)
	{
		this->showErr("Send Error");
		return -1;
	}
	return 1;
}

void tcp_client::showErr(const char *err)
{
	perror(err);
	exit(1);
}

void tcp_client::closeClientSocket()
{
	close(this->clientSocket);
}
