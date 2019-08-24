//
// Created by Beniamin on 2017-08-29.
//

#include "ClientClass.h"

using namespace std;

extern "C"{


ClientClass::ClientClass() {

    additional_data.addrlen = sizeof(sockaddr);

    client_data.my_socekt_fd = socket(AF_INET, SOCK_STREAM, 0);

    memset( &server_data.server_addr, 0, sizeof(server_data.server_addr));

    server_data.server_addr.sin_family = AF_INET;
    server_data.server_addr.sin_port = htons(8989);
    inet_pton(AF_INET, "192.168.43.1", &server_data.server_addr.sin_addr);
}

bool ClientClass::ConnectToServer(int port, char * ip_address){

    server_data.ip_address = ip_address;
    server_data.port = port;

    memset( &server_data.server_addr, 0, sizeof(server_data.server_addr));

    server_data.server_addr.sin_family = AF_INET;
    server_data.server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip_address, &server_data.server_addr.sin_addr);

    if(connect(client_data.my_socekt_fd, (sockaddr *)&server_data.server_addr, sizeof(server_data.server_addr))
            == -1){
        __android_log_print(ANDROID_LOG_VERBOSE, "Client:", "Connection error", 1);
        return false;
    }else{
        __android_log_print(ANDROID_LOG_VERBOSE, "Client:", "Connected", 1);
    }

    return true;
}

bool ClientClass::Receive(){
    __android_log_print(ANDROID_LOG_VERBOSE, "Client:", "RECEIVING DATA", 1);
    if( (received_data.length = recv(client_data.my_socekt_fd, &received_data.buffer, 1024, 0) ) > 0) {
       // received_data.buffer[received_data.length] = '\0';
        __android_log_print(ANDROID_LOG_VERBOSE, "RECEIVED :", received_data.buffer, 1);
        return true;
    }

    return false;
};

void ClientClass::SendMessage(char *message, int n_length) {

    int l_sent = 0;
    char* l_buffer = new char[n_length+1];
    for(int i = 0; i < n_length; i++){
        l_buffer[i] = message[i];
    }
    l_buffer[n_length] = '\0';
    l_sent = send(client_data.my_socekt_fd, l_buffer, n_length+1, 0);
    __android_log_print(ANDROID_LOG_VERBOSE, "WYSLANO", l_buffer , 1);

    delete [] l_buffer;
}

char* ClientClass::GetMessage() {
    string retrunValue;
    char* l_buffer = new char[received_data.length+1];
    for(int i = 0; i < received_data.length; i++){
        l_buffer[i] = received_data.buffer[i];
    }
    l_buffer[received_data.length] = '\0';
    retrunValue = l_buffer;
    delete [] l_buffer;
    return (char*)retrunValue.c_str();

};

}