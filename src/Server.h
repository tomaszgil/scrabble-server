#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include <vector>
#include "Player.h"
#include "Room.h"
#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <sys/fcntl.h>
#include <string>
#include <cstring>
#include <pthread.h>

class Server {

    std::vector<Player> players;
    std::vector<Room> rooms;
    char *server_address = "127.0.0.1";
    short service_port = 12345;
    int socket_desc;
    struct sockaddr_in server_struct;


public:
    Server();
    void createSocket();
    void bindSocket();
    void listenForConnections();
    void acceptConnection();
    void run();
    static void* handleClient(void* arg);

    struct pthread_data{
        int client_desc;
    };
};


#endif //SERVER_SERVER_H
