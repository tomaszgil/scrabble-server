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
    std::string username;
    std::string score;
    int socket_desc;
    std::string room;
    bool turn;

public:
    bool isTurn() const;
    void setTurn(bool turn);
    int getSocket_desc() const;
    void setSocket_desc(int socket_desc);
    const std::string &getScore() const;
    void setScore(const std::string &score);
    const std::string &getRoom() const;
    void setRoom(const std::string &room);
    Player(struct sockaddr_in player, socklen_t player_size, int socket_desc);
    Player(int a);
    const std::string &getUsername() const;
    void setUsername(const std::string &username);

};


#endif //SERVER_PLAYER_H
