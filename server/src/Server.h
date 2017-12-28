#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include <vector>
#include "Player.h"
#include "Room.h"


class Server {

    std::vector<Player> players;
    std::vector<Room> rooms;

public:
    Server();
    void run();

};


#endif //SERVER_SERVER_H
