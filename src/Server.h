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
#include <thread>

class Server {

    std::vector<Player> players;

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
    void createRooms();
    static void *handleClient(void *data);
    static void receiveUsername(int desc, Player player);
    static void sendAvaibleRooms(int desc);


    struct pthread_data {
        int client_desc;
        Player player;
    };

    struct room_data{
        std::string room_names[5];
        int free_slots[5];
    };

    static std::vector<Room> rooms;
};


#endif //SERVER_SERVER_H
