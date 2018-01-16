#ifndef SERVER_PLAYER_H
#define SERVER_PLAYER_H

#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>

class Player {

    struct sockaddr_in player_struct;
    socklen_t player_struct_size;

public:
    Player(struct sockaddr_in player, socklen_t player_size);
    Player(int a);

};


#endif //SERVER_PLAYER_H
