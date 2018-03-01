#ifndef SERVER_ROOM_H
#define SERVER_ROOM_H

#include <cstdio>
#include <string>
#include <cstring>
#include <vector>
#include "Player.h"


class Room {

private:
    int freeSlots;
    std::string name;

public:
    std::vector<Player> players;

    Room();
    Room(std::string name, int freeSlots);
    const std::string &getName() const;

    void setName(const std::string &name);

    int getFreeSlots() const;

    void setFreeSlots(int freeSlots);

    void addPlayer(Player &player);

};


#endif //SERVER_ROOM_H
