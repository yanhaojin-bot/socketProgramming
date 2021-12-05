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
#define PORT 26592 // the port client will be connecting to

using namespace std;

int main(int argc, char const *argv[])
{
	if (argc < 2 || argc > 3)
	{
		printf("No enough parameters or too many parameters");
		return -1;
	}
	int v = 0, size = argc - 1;
	char *input1 = (char *)malloc(v);
	char *input2 = (char *)malloc(v);
	input1 = (char *)realloc(input1, (v + strlen(argv[1])));
	strcat(input1, argv[1]);
	if (size == 2)
	{
		input2 = (char *)realloc(input2, (v + strlen(argv[2])));
		strcat(input2, argv[2]);
	}
	// read arguments 	
	printf("The client is up and running.\n");
	
	// connect serverC
	tcp_client client(HOSTNAME, PORT);
	client.start();
	client.sendData(to_string(size));
	client.sendData(input1);
	if (size == 1) {
		printf("The client sent %s to the Central server.\n", input1);
	}
	else if (size == 2) {
		client.sendData(input2);
		printf("The client sent %s , %s to the Central server.\n", input1, input2);
	}
	// receive data from serverC and print 
	int n;
	n = client.recvData();
	while (trim(client.recvString) != "end" && n > 0)
	{
		cout << client.recvString;
		n = client.recvData();
	}
	client.closeClientSocket();
	return 0;
}
