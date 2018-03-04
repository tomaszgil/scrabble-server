#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include <vector>
#include "Player.h"
#include "Room.h"
#include "Game.h"
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
#include <unistd.h>

class Server {
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
    void createGames();
    static void getNewLetters(Player &player);
    static void *handleClient(void *data);
    static void receiveUsername(int desc, Player &player);
    static void receiveSelectedRoom(int desc, Player &player);
    static void sendAvaibleRooms(int desc);
    static void sendBoard(int desc, Player &player, int code = 0);
    static void sendAvaibleLetters(int desc, Player &player, int code =0);
    static bool sendStringToClient(int desc, std::string &message, int size =0);
    static void sendPlayersFromCurrentRoom(int desc, Player &player, int code =0);
    static void receiveUserMove(int desc, Player &player);
    static void sendMoveToOtherPlayers(int desc, Player &player);
    static void swapLetters(int desc, Player &player);
    static void quitRoom(int desc, Player &player);

    struct pthread_data {
        int client_desc;
        Player &player;
    };

    struct room_data{
        std::string room_names[5];
        int free_slots[5];
    };

    static std::vector<Room> rooms;
    static std::vector<Game> games;
    static std::vector<Player> players;
};


#endif //SERVER_SERVER_H
