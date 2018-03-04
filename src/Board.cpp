#include "Board.h"

Board::Board() {
    for(int i =0; i<15; i++)
        for(int j=0; j<15; j++) {
            this->board[i][j]='0';
        }
}

void Board::clear() {
    for(int i =0; i<15; i++)
        for(int j=0; j<15; j++) {
            this->board[i][j]='0';
        }
}
