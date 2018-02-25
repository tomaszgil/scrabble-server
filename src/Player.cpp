#include "Player.h"

Player::Player(struct sockaddr_in player, socklen_t player_size) {

    player_struct = player;
    player_struct_size = player_size;

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
