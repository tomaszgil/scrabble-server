#include "Game.h"
#include <map>
#include <random>

Game::Game(Room room, Board board) {
    this->room = room;
    this->board = board;
    setupLetters();
}

void Game::clear() {
    room.players.clear();
    board.clear();
    setupLetters();
}

void Game::setupLetters() {
    struct cmp_char
    {
        bool operator()(char const a, char const b)
        {
            return a < b;
        }
    };

    std::map<char, int, cmp_char> distribution;
    distribution.insert(std::pair<char, int>('A', 9));
    distribution.insert(std::pair<char, int>('B', 2));
    distribution.insert(std::pair<char, int>('C', 2));
    distribution.insert(std::pair<char, int>('D', 4));
    distribution.insert(std::pair<char, int>('E', 12));
    distribution.insert(std::pair<char, int>('F', 2));
    distribution.insert(std::pair<char, int>('G', 3));
    distribution.insert(std::pair<char, int>('H', 2));
    distribution.insert(std::pair<char, int>('I', 9));
    distribution.insert(std::pair<char, int>('J', 1));
    distribution.insert(std::pair<char, int>('K', 1));
    distribution.insert(std::pair<char, int>('M', 2));
    distribution.insert(std::pair<char, int>('N', 6));
    distribution.insert(std::pair<char, int>('O', 8));
    distribution.insert(std::pair<char, int>('P', 2));
    distribution.insert(std::pair<char, int>('Q', 1));
    distribution.insert(std::pair<char, int>('R', 6));
    distribution.insert(std::pair<char, int>('S', 4));
    distribution.insert(std::pair<char, int>('T', 6));
    distribution.insert(std::pair<char, int>('U', 4));
    distribution.insert(std::pair<char, int>('V', 2));
    distribution.insert(std::pair<char, int>('W', 2));
    distribution.insert(std::pair<char, int>('X', 1));
    distribution.insert(std::pair<char, int>('Y', 2));
    distribution.insert(std::pair<char, int>('Z', 1));

    std::map<char, int>::iterator it = distribution.begin();

    while (it != distribution.end())
    {
        char letter = it -> first;
        int count = it -> second;

        for (int i = 0; i < count; i++) {
            letters.push_back(letter);
        }

        it++;
    }
}

char Game::takeLetter() {
    char letter = '0';
    if (!letters.empty()) {
        unsigned long idx = random() % letters.size();
        letter = letters[idx];
        letters.erase(letters.begin() + idx);
    }

    return letter;
}

void Game::insertLetter(char letter) {
    letters.push_back(letter);
}

