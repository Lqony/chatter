//
// Created by Beniamin on 2017-08-22.
//

#ifndef BENQMESSENGER_SERVERCLASS_H
#define BENQMESSENGER_SERVERCLASS_H

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
#include <sstream>

#define BUFFOR_SIZE 1024
#define DEFAULT_SOCKETS 5000
#define LISTENER_BACK_LOG 10

using namespace std;


extern "C"{

enum SOCKET_STATES { NOT_CONNECTED = 0, CONNECTED = 1 };
enum ACCEPT_DATA_RESULT { DISCONNECTED = 0, ERROR = 1, TOO_MUCH_DATA = 2 /*When data received is bigger than buffer size*/,
                            DATA_NOT_COMPLETE = 3 /*WHEN LESS DATA CAME THAN PACKAGE STATES IN FIRT 2 BYTES*/,
                            DATA_COMPLETE = 4};

struct Fd_Data{
    int server_fd; //listener
    int new_fd; //for new connections
    fd_set main_list;
    fd_set help_list;
    int fd_max;
};

struct Addr_Data{
    sockaddr_in server_address;
    sockaddr_in client_address;
};

struct Additional_Data{
    int yes = 1;
    int addrlen;
    bool received = false;
};

struct Buffer_Data{
    char *buffer;
    int size;
};

struct Server_Settings{
    int port = 8989;
    string ip_address;
    int back_log = 10; //max connections awaiting to accept at one time
};


class ServerClass {

    private:
    Fd_Data fd_data;
    Addr_Data addr_data;
    Buffer_Data buffer_data;
    Additional_Data additional_data;
    Server_Settings server_settings;

    public:
    ServerClass(char* ip_address);

    void ManageConnections();
    void SendMessage(char *n_message, int n_length, int exclude_fd = -1);
    bool Received() {
        if(additional_data.received) {
            additional_data.received =false;
            return true;
        }
        return false;
    }
    char* ReceivedData();

    int Port() { return server_settings.port; }

};

class BenqSocket{

    private:

        int fd;
        int state;
        int received_data_lenght;
        int messages_received;

        //Client's address, where connections come fron
        sockaddr_in address;

        char *received_data;

    public:

        BenqSocket(); //INIT
        ~BenqSocket();

        void Connect(int n_fd, sockaddr_in n_address); //ONLY SETS UP SOCK STATE AND SAVES SERVER DATA AND SETS IT TO DEAFULS
        void Disconnect(); //ONLY CHANGES SOCK STATE

        int AcceptData(int length, char*buffer);
        int MessagesReceived() { return messages_received; }
        int LastMessage(char *n_buffer);
        int State() { return state; }
        int Fd() { return fd; }

};

struct BenqSockets{
    int max_amount;
    BenqSocket *sockets;
};

struct Socket_File_Descriptors{
    fd_set main_set;
    fd_set help_set;
    int max_fd;
};

struct BenqServerInfo{
    sockaddr_in address; //SERVER ADDRESS
    int state;
    int listener_id; //IN SOCKETS TABlE
    Socket_File_Descriptors fd_data;
};

struct BenqDataBuffer{
    char *buffer;
    int current_size;
    int max_size;
};

class BenqServerClass {

    private:
        BenqSockets sockets;
        BenqServerInfo info ;
        BenqDataBuffer buffer;

        int CreateSocket(int new_fd, sockaddr_in address);
        void DeleteSocket(int fd);
    public:

        BenqServerClass(sockaddr_in address, int max_clients_amount); //set everything to default
        void StartServer(); //Init and set server to listen
        void Run(); //Server Loop
        void ReceiveData(int fd);

};

};
#endif //BENQMESSENGER_SERVERCLASS_H
