#include "Player.h"

Player::Player(struct sockaddr_in player, socklen_t player_size) {

    player_struct = player;
    player_struct_size = player_size;
    std::cout << "UDALO SIE MNIE ZROBIC " << std::endl;

}

Player::Player(int a) {
    std::cout<<a<<std::endl;
}