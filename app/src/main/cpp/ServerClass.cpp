//
// Created by Beniamin on 2017-08-22.
//


#include "ServerClass.h"


using namespace std;

extern "C"{

    ServerClass::ServerClass(char* ip_address) {

        server_settings.ip_address = ip_address;

        buffer_data.buffer = new char[1024];
        additional_data.addrlen = sizeof(addr_data.client_address);

        FD_ZERO(&fd_data.main_list);
        FD_ZERO(&fd_data.help_list);

        fd_data.server_fd = socket(PF_INET, SOCK_STREAM, 0);
        setsockopt(fd_data.server_fd, SOL_SOCKET, SO_REUSEADDR, &additional_data.yes, sizeof(additional_data.yes));
        addr_data.server_address.sin_family = AF_INET;
        addr_data.server_address.sin_port = htons(8989);
        inet_pton(AF_INET, (char*)server_settings.ip_address.c_str(), &addr_data.server_address.sin_addr);

        bind(fd_data.server_fd, (struct sockaddr*)&addr_data.server_address, sizeof(addr_data.server_address));
        listen(fd_data.server_fd, server_settings.back_log);

        FD_SET(fd_data.server_fd, &fd_data.main_list);
        fd_data.fd_max = fd_data.server_fd;
        fd_data.help_list = fd_data.main_list;

        stringstream ss, cc;
        int m = 20;
        int n = 0;
        int x = 6;
        int y = 0;
        char *l_buffer;
        ss.clear();
        ss.str("");
        cc.clear();
        cc.str("");
        l_buffer = new char[1024];

        memcpy(l_buffer, &m, sizeof(int));
        memcpy(&l_buffer[sizeof(int)], &x, sizeof(int));
        //send data
        memcpy(&n, l_buffer, sizeof(int));
        memcpy(&y, &l_buffer[sizeof(int)], sizeof(int));

        if(n == 15){
            __android_log_print(ANDROID_LOG_VERBOSE, "N = ", "15 ", 1);
        }else{
            __android_log_print(ANDROID_LOG_VERBOSE, "N != ", "15 ", 1);
            ss << n;
            __android_log_print(ANDROID_LOG_VERBOSE, "N = ", ss.str().c_str(), 1);
        }

        if(y == 10){
            __android_log_print(ANDROID_LOG_VERBOSE, "Y = ", "10 ", 1);
        }else{
            __android_log_print(ANDROID_LOG_VERBOSE, "Y != ", "10 ", 1);
            cc << y;
            __android_log_print(ANDROID_LOG_VERBOSE, "Y = ", cc.str().c_str(), 1);
        }

      __android_log_print(ANDROID_LOG_VERBOSE, "DATA SAYS", l_buffer, 1);
      delete []  l_buffer;

    }

    void ServerClass::ManageConnections() {

        fd_data.help_list = fd_data.main_list;

        select(fd_data.fd_max+1, &fd_data.help_list, NULL, NULL, NULL);

        for(int i = 0; i <= fd_data.fd_max; i++){
            if(FD_ISSET(i, &fd_data.help_list)){
                if(i == fd_data.server_fd){
                    fd_data.new_fd = accept(fd_data.server_fd, (struct sockaddr*)&addr_data.server_address,
                                            (socklen_t*)&additional_data.addrlen);
                    FD_SET(fd_data.new_fd, &fd_data.main_list);
                    if(fd_data.new_fd > fd_data.fd_max){
                        fd_data.fd_max = fd_data.new_fd ;
                    }
                    __android_log_print(ANDROID_LOG_VERBOSE, "Server:", "NOWY KLIENT", 1);
                }else{
                    //WE GOT SOME DATA OR DISCONECCTED
                    buffer_data.size = recv(i, buffer_data.buffer, 1024, 0);
                    if(buffer_data.size > 0){
                        __android_log_print(ANDROID_LOG_VERBOSE, "Server:", "ODEBRANO WIADOMOSC ", 1);
                        additional_data.received = true;
                       SendMessage(buffer_data.buffer, buffer_data.size, i);
                    }
                }
            }
        }


    }

    void ServerClass::SendMessage(char *n_message, int n_length, int exclude_fd) {
        int l_sent = 0;
        char* l_buffer = new char[n_length+1];
        for(int i = 0; i < n_length; i++){
            l_buffer[i] = n_message[i];
        }
        l_buffer[n_length] = '\0';
        for(int i = 0; i <= fd_data.fd_max; i++) {
            if (FD_ISSET(i, &fd_data.main_list)){
                if (i != fd_data.server_fd) {
                    if(exclude_fd == -1 || exclude_fd != i) {
                        l_sent = send(i, l_buffer, n_length + 1, 0);
                        __android_log_print(ANDROID_LOG_VERBOSE, "WYSLANO", l_buffer, 1);
                    }
                }
            }
        }
        delete [] l_buffer;
    }

    char *ServerClass::ReceivedData() {
    string retrunValue;
    char* l_buffer = new char[buffer_data.size+1];
    for(int i = 0; i < buffer_data.size; i++){
        l_buffer[i] = buffer_data.buffer[i];
    }
    l_buffer[buffer_data.size] = '\0';
    retrunValue = l_buffer;
    delete [] l_buffer;
    return (char*)retrunValue.c_str();
}

//----------------------BENQ SOCKET-----------------------------------------------------

    BenqSocket::BenqSocket() {
        state = NOT_CONNECTED;
        received_data_lenght = 0;
        messages_received = 0;
        received_data = new char[ BUFFOR_SIZE * 10];
        //SO EVEN IF TCP SPLITS/JOINS PACKET YOU CAN TAKE UP TO TEN MORE DATA THAN TCP CAN SEND AT ONCE
        memset( &address, 0, sizeof(address));
    }

    BenqSocket::~BenqSocket() {
        delete [] received_data;
    }

    void BenqSocket::Connect(int n_fd, sockaddr_in n_address) {

        state = CONNECTED;
        received_data_lenght = 0;
        messages_received = 0;
        fd = n_fd;
        address.sin_addr.s_addr = n_address.sin_addr.s_addr;
        address.sin_port = n_address.sin_port;
    }

    void BenqSocket::Disconnect() {
        state = NOT_CONNECTED;
    }

    int BenqSocket::AcceptData(int length, char *buffer) {
        unsigned short l_message_length = 0;
        unsigned short l_message_beggining = 0;
        unsigned short l_data_proccesed = 0;

        if(length == 0){
            return DISCONNECTED;
        }else if(length < 0){
            return ERROR;
        }else if(length >= BUFFOR_SIZE){
            return TOO_MUCH_DATA;
        }
        else{
            if(received_data_lenght + length > ((BUFFOR_SIZE * 10)-1) ){
                //clear the las trash if any
                received_data_lenght = 0;
                while(true){
                    memcpy(&l_message_length, &received_data[l_message_beggining], sizeof(l_message_length));
                    l_data_proccesed += l_message_length;
                    if(l_data_proccesed >= received_data_lenght){

                        break;
                    }
                    received_data_lenght += l_message_length;
                    l_message_beggining += l_message_length;
                }
                return TOO_MUCH_DATA;
            }else {
                memcpy(&received_data[received_data_lenght], buffer, length);
                received_data_lenght += length;

                while(true){
                    memcpy(&l_message_length, &received_data[l_message_beggining], sizeof(l_message_length));
                    l_data_proccesed += l_message_length;

                    if(l_data_proccesed > received_data_lenght){
                        break;
                    }else if(l_data_proccesed == received_data_lenght){
                        messages_received++;
                        break;
                    }

                    l_message_beggining += l_message_length;
                    messages_received++;
                }

                if(messages_received > 0){
                    return DATA_COMPLETE;
                }else{
                    return DATA_NOT_COMPLETE;
                }
            }
        }
    }


    int BenqSocket::LastMessage(char *n_buffer) {
        unsigned short l_length = 0;
        char *l_helper = NULL;

        if(messages_received > 0){
            memcpy(&l_length, received_data, sizeof(l_length));
            memcpy(n_buffer, received_data, l_length);
            messages_received--;

            if(l_length < received_data_lenght){
                //DELETE RETURNED MESSAGE AND MOVE REST DATA TOWARD BEGGINING OF THE BUFFER

                l_helper = new char[received_data_lenght - l_length];
                memcpy(l_helper, &received_data[l_length], sizeof(received_data_lenght - l_length));
                memcpy(received_data, l_helper, sizeof(received_data_lenght - l_length));
            }
        }

        delete [] l_helper;

        return l_length;
    }

//----------------------BENQ SOCKET-----------------------------------------------------

    BenqServerClass::BenqServerClass(sockaddr_in address, int max_clients_amount) {
        sockets.max_amount = max_clients_amount;
        sockets.sockets = new BenqSocket[max_clients_amount];
        info.address.sin_family = AF_INET;
        info.address.sin_port = address.sin_port;
        info.address.sin_addr.s_addr = address.sin_addr.s_addr;
        info.state = NOT_CONNECTED;
        info.fd_data.max_fd = 0;

        FD_ZERO(&info.fd_data.main_set);
        FD_ZERO(&info.fd_data.help_set);

        buffer.max_size = BUFFOR_SIZE;
        buffer.current_size = 0;
        buffer.buffer = new char[BUFFOR_SIZE];

    }

    int BenqServerClass::CreateSocket(int new_fd, sockaddr_in address) {

        for(int i = 0; i < sockets.max_amount; i++){
            if(sockets.sockets[i].State() == NOT_CONNECTED){
                sockets.sockets[i].Connect(new_fd, address);
                return i;
            }
        }

        close(new_fd);
        return -1;
    }

    void BenqServerClass::StartServer() {
        int ignore = 1; //IGNORE BUT NECESSARY IN SECSOCKOPT
        int new_fd;

        new_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        info.listener_id = CreateSocket(new_fd, info.address);
        //TODO if CreateSocket return -1 it is BAD
        setsockopt(sockets.sockets[info.listener_id].Fd(), SOL_SOCKET, SO_REUSEADDR, &ignore, sizeof(ignore));

        //bind
        bind(sockets.sockets[info.listener_id].Fd(), (struct sockaddr*)&info.address, sizeof(info.address));

        //listen
        listen(sockets.sockets[info.listener_id].Fd(), LISTENER_BACK_LOG);

        FD_SET(sockets.sockets[info.listener_id].Fd(), &info.fd_data.main_set);
        if(sockets.sockets[info.listener_id].Fd() > info.fd_data.max_fd){
            info.fd_data.max_fd = sockets.sockets[info.listener_id].Fd();
        }

        info.state = CONNECTED;
    }

    void BenqServerClass::DeleteSocket(int fd) {
        //FIND WHOM FD BELONGS TO
        for(int i = 0; i < sockets.max_amount; i++){
            if(sockets.sockets[i].Fd() == i){
                sockets.sockets[i].Disconnect();
                FD_CLR(fd, &info.fd_data.main_set);
                close(fd);
                break;
            }
        }
    }

    void BenqServerClass::ReceiveData(int fd) {
        //RECV WAS ALREADY CALLED BEFORE THIS FUNCTION, SO ALL DATA NAD INFO IS IN "buffer"
        if(buffer.current_size < 0){
            //TODO ERROR
        }else if(buffer.current_size == 0){
            DeleteSocket(fd);
        }else if(buffer.current_size > 0){
            //FIND WHOM FD BELONGS TO
            for(int i = 0; i < sockets.max_amount; i++){
                if(sockets.sockets[i].Fd() == i){
                    sockets.sockets[i].AcceptData(buffer.current_size, buffer.buffer);
                    break;
                }
            }
        }
    }

    void BenqServerClass::Run() {
        int new_fd;
        sockaddr_in new_address;
        int address_len = sizeof(new_address);

        info.fd_data.help_set = info.fd_data.main_set;
        select(info.fd_data.max_fd+1, &info.fd_data.help_set, NULL, NULL, NULL);

        //CHECK LISTENER ACTIVITY
        if(FD_ISSET(sockets.sockets[info.listener_id].Fd(), &info.fd_data.help_set)){
            //NEW CONNECTION
            new_fd = accept(sockets.sockets[info.listener_id].Fd(), (struct sockaddr*)&new_address,
                            (socklen_t*)&address_len);

            if(new_fd < 0){
                //todo ERROR
            }else{
                FD_SET(new_fd, &info.fd_data.main_set);
                if(new_fd > info.fd_data.max_fd){
                    info.fd_data.max_fd = new_fd;

                    CreateSocket(new_fd, new_address);
                }
            }
        }

        //CHECK ALL OTHER SOCKETS ACTIVITY
        for(int i = 0; i < info.fd_data.max_fd; i++){
            //WE ALREADY HAVE CHECK SERVER
            if(i != sockets.sockets[info.listener_id].Fd()){
                if(FD_ISSET(i, &info.fd_data.help_set)){
                    buffer.current_size = recv(i, buffer.buffer, 1024, 0);
                    ReceiveData(i);
                }
            }
        }
    }
}



