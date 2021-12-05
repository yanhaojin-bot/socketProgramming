
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;


// citation: https://www.binarytides.com/code-a-simple-socket-client-class-in-c/
class tcp_server
{
private:
    void showErr(const char *);
    int bufferSize;
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
public:
    string recvString;
    tcp_server(int);
    int accpet();
    int recvData();
    int sendData(string);
    int closeServer();
};

tcp_server::tcp_server(int listenPort)
{
    this->bufferSize = 1024;
    if ((this->server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    this->address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(listenPort);

    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
}

int tcp_server::accpet()
{
    if ((this->new_socket = accept(server_fd, (struct sockaddr *)&address,
                                   (socklen_t *)&addrlen)) < 0)
    {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }
}

int tcp_server::recvData()
{
    // receive length of data
    int n;
    int recvLength = 0;
    n = read(this->new_socket, &recvLength, sizeof(recvLength));
    if (n < 0) {
        perror("receive length failed");
    }
    recvLength = ntohl(recvLength);

    // receive data;
    char buffer[this->bufferSize];
    memset(buffer, 0, this->bufferSize);
    n = read(this->new_socket, buffer, recvLength);
    if (n < 0) {
        perror("receive content failed");
    }
    recvString = buffer;
    return n;
}

int tcp_server::sendData(string content)
{
    int len = htonl(content.size());
    int size = content.size();
    int bufferSize = this->bufferSize - 1;
    int sendPosition = 0;
    // split content 
    while (size > 0)
    {
        // send data of length;
        // split messages into packets with max_size;
        int sendLen =  min(bufferSize, size);
        if ((write(this->new_socket, &sendLen, sizeof(sendLen))) < 0)
        {
            this->showErr("Send length Error");
            return -1;
        }
        string sendContent = content;
        if (sendLen != size) {
            sendContent = content.substr(0, sendLen);
            content = content.substr(sendLen);

        }
        size -= sendLen;
        int n;
        n = write(this->new_socket, sendContent.c_str(), sendContent.size());
        if (n < 0)
        {
            this->showErr("Send content Error");
            return -1;
        }
        
    }



    return 0;
}

int tcp_server::closeServer()
{
    this->recvString = "";
    close(this->new_socket);
}

void tcp_server::showErr(const char *error)
{
    perror(error);
    exit(1);
}