#include "Player.h"

Player::Player(struct sockaddr_in player, socklen_t player_size) {
    this->player_struct = player;
    this->player_struct_size = player_size;
}
