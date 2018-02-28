#include "Room.h"
#include "Server.h"

Room::Room() {

}

Room::Room(std::string roomName, int slots) {
    name = roomName;
    freeSlots = slots;
}

const std::string &Room::getName() const {
    return name;
}

void Room::setName(const std::string &name) {
    Room::name = name;
}

int Room::getFreeSlots() const {
    return freeSlots;
}

void Room::setFreeSlots(int freeSlots) {
    Room::freeSlots = freeSlots;
}

void Room::addPlayer(Player &player) {
    Room::players.push_back(player);
}
