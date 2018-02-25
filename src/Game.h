#ifndef SERVER_GAME_H
#define SERVER_GAME_H
#include <vector>
#include "Player.h"
#include "Room.h"
#include "Board.h"

class Game {
public:
    std::vector<Player> players;
    Room room;
    Board board;

    Game(Room room, Board board);
};


#endif //SERVER_GAME_H
