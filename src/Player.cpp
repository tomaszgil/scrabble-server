#include "Player.h"

Player::Player(struct sockaddr_in player, socklen_t player_size) {

    player_struct = player;
    player_struct_size = player_size;
    score = "000";

}

Player::Player(int a) {
    std::cout<<a<<std::endl;
}

const std::string &Player::getUsername() const {
    return username;
}

void Player::setUsername(const std::string &username) {
    Player::username = username;
}

const Room &Player::getRoom() const {
    return room;
}

void Player::setRoom(const Room &room) {
    Player::room = room;
}

const std::string &Player::getScore() const {
    return score;
}

void Player::setScore(const std::string &score) {
    Player::score = score;
}

