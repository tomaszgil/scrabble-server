#ifndef SERVER_PLAYER_H
#define SERVER_PLAYER_H

#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include "Room.h"

class Player {

    struct sockaddr_in player_struct;
    socklen_t player_struct_size;
    std::string username;
    int score;
    Room room;
public:
    const Room &getRoom() const;

    void setRoom(const Room &room);


public:
    Player(struct sockaddr_in player, socklen_t player_size);
    Player(int a);
    const std::string &getUsername() const;
    void setUsername(const std::string &username);

};


#endif //SERVER_PLAYER_H
