#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include "tcp_client.cpp"
#include "util.cpp"
#define HOSTNAME "127.0.0.1"
#define PORT 25592 // the port client will be connecting to

using namespace std;


int main(int argc, char const *argv[])
{
	if (argc != 2)
	{
		printf("No enough parameters or too many parameters");
		return -1;
	}
	int v = 0;

	char *input1 = (char *)malloc(v);
	input1 = (char *)realloc(input1, (v + strlen(argv[1])));
	strcat(input1, argv[1]);
	// read arguments 
	printf("The client is up and running.\n");
	
	// connect serverC
	tcp_client client(HOSTNAME, PORT);
	client.start();
	client.sendData(input1);
	printf("The client sent %s to the Central server.\n", input1);

	// receive data from serverC and print 
	int n ;
	client.recvData();	
	while(trim(client.recvString) != "end")
	{
		cout << client.recvString ;
		client.recvData();
	}
	client.closeClientSocket();
	return 0;
}
