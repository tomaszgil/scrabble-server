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
    std::vector<char> letters;

    Game(Room room, Board board);
    char takeLetter();
    void insertLetter(char letter);
    void clear();

private:
    void setupLetters();
};


#endif //SERVER_GAME_H
