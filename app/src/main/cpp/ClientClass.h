//
// Created by Beniamin on 2017-08-29.
//

#ifndef BENQMESSENGER_CLIENTCLASS_H
#define BENQMESSENGER_CLIENTCLASS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <android/log.h>

using namespace std;

extern "C"{

struct Server_Data{
    int port;
    string ip_address;
    sockaddr_in server_addr;
};

struct Client_Data{
    int my_socekt_fd;
};

struct Additional_data{
    int addrlen;
};

struct Received_Data{
    char buffer[1024];
    int length;
};

struct Buffer_data{
    char *buffer;
    int size;
};

class ClientClass {
    private:

    Server_Data server_data;
    Client_Data client_data;
    Additional_data additional_data;
    Received_Data received_data;
    Buffer_data buffer_data;

    public:
        ClientClass();

    void SendMessage(char *message, int n_length);

    bool ConnectToServer(int port, char * ip_address);
    char *GetMessage();
    bool Receive();
};

};


#endif //BENQMESSENGER_CLIENTCLASS_H
