#include "Player.h"

Player::Player(struct sockaddr_in player, socklen_t player_size, int desc) {

    player_struct = player;
    player_struct_size = player_size;
    score = "000";
    socket_desc = desc;
    turn = false;

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

const std::string &Player::getRoom() const {
    return room;
}

void Player::setRoom(const std::string &room) {
    Player::room = room;
}

const std::string &Player::getScore() const {
    return score;
}

void Player::setScore(const std::string &score) {
    Player::score = score;
}

int Player::getSocket_desc() const {
    return socket_desc;
}

void Player::setSocket_desc(int socket_desc) {
    Player::socket_desc = socket_desc;
}

bool Player::isTurn() const {
    return turn;
}

void Player::setTurn(bool turn) {
    Player::turn = turn;
}


