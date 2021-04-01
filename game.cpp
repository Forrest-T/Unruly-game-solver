/* Copyright 2018, Forrest Timour */

#include <cassert>
#include <stack>
#include <vector>
#include <iostream>
#include <algorithm>

#include "game.hpp"
#include "solve.hpp"

/* Game class functions */

Game::Game(unsigned int size, Tile state[]):
numTiles(0), size(size), rowB(size, 0), colB(size, 0), rowW(size, 0), colW(size, 0)
{
    Tile temp;
    board = new Tile*[size];
    for (unsigned int r = 0; r < size; r++) {
        board[r] = new Tile[size];
        for (unsigned int c = 0; c < size; c++) {
            temp = state[size*r+c];
            board[r][c] = temp;
            if (temp == B) {
                numTiles++;
                rowB[r]++;
                colB[c]++;
            } else if (temp == W) {
                numTiles++;
                rowW[r]++;
                colW[c]++;
            }
        }
    }
}
Game::~Game() {
    for (unsigned int i = 0; i < size; i++)
        delete[] board[i];
    delete[] board;
}

bool Game::verifySolved() {
    return numTiles == size*size && checkrep();
}

std::ostream & operator<<(std::ostream &os, Game const &game) {
    for (unsigned int r = 0; r < game.size; r++) {
        os << " ";
        for (unsigned int c = 0; c < game.size; c++) {
            if (game.board[r][c] == B)
                os << "b";
            else if (game.board[r][c] == W)
                os << "w";
            else
                os << "-";
        }
        os << std::endl;
    }
    return os;
}

void Game::apply(const Move &m) {
    board[m.row][m.col] = m.color;
    auto &row = (m.color == B)? rowB : rowW;
    auto &col = (m.color == B)? colB : colW;
    row[m.row]++;
    col[m.col]++;
    numTiles++;
}

void Game::undo(const Move &m) {
    assert(m.color == board[m.row][m.col]);
    auto &row = (m.color == B)? rowB : rowW;
    auto &col = (m.color == B)? colB : colW;
    board[m.row][m.col] = E;
    row[m.row]--;
    col[m.col]--;
    numTiles--;
}

bool Game::checkrep() {
    // check number of each color per row/col
    for (unsigned int i = 0; i < size; i++) {
        if (rowB[i] > size/2) return false;
        if (colB[i] > size/2) return false;
        if (rowW[i] > size/2) return false;
        if (colW[i] > size/2) return false;
    }

    // check row count
    unsigned int count = 0;
    for (unsigned int r = 0; r < size; r++) {
        unsigned int rowCountB = 0;
        unsigned int rowCountW = 0;
        for (unsigned int c = 0; c < size; c++) {
            if (board[r][c] == B) {
                count++;
                rowCountB++;
            }
            if (board[r][c] == W) {
                count++;
                rowCountW++;
            }
        }
        if (rowB[r] != rowCountB || rowW[r] != rowCountW) return false;
    }
    // check total count
    if (count != numTiles) return false;
    // check col count
    for (unsigned int c = 0; c < size; c++) {
        unsigned int colCountB = 0;
        unsigned int colCountW = 0;
        for (unsigned int r = 0; r < size; r++) {
            if (board[r][c] == B) {
                colCountB++;
            }
            if (board[r][c] == W) {
                colCountW++;
            }
        }
        if (colB[c] != colCountB || colW[c] != colCountW) return false;
    }
    return true;
}

std::vector<Move> *Game::generateMoves() {
    std::vector<Move> *moves = new std::vector<Move>();
    moves->reserve(2*(size*size-numTiles));
    for (unsigned int r = 0; r < size; r++) {
        for (unsigned int c = 0; c < size; c++) {
            if (board[r][c] != E)
                continue;
            if (isValid(B, r, c))
                moves->push_back(Move(B, r, c));
            if (isValid(W, r, c))
                moves->push_back(Move(W, r, c));
        }
    }
    return moves;
}

bool Game::isWinnable() {
    for (unsigned int i = 0; i < size; i++)
        if (rowB[i] > size/2 || rowW[i] > size/2 ||
            colB[i] > size/2 || colW[i] > size/2 ) { return false; }
    for (unsigned int r = 0; r < size; r++) {
        for (unsigned int c = 0; c < size; c++)
            if (board[r][c] == E && !isValid(W, r, c) && !isValid(B, r, c))
                return false;
    }
    return true;
}

bool Game::isValid(Tile color, unsigned int r, unsigned int c) {
    if (r >= size || c >= size || color == E) return false;
    auto const &row = (color == B)? rowB : rowW;
    auto const &col = (color == B)? colB : colW;
    // check for row and col saturation
    if (row[r] >= size/2 || col[c] >= size/2) return false;
    // check for triple in row
    unsigned int count = 1;
    if (r > 0 && board[r-1][c] == color)
        count += (r > 1 && board[r-2][c] == color)? 2 : 1;
    if (r < size-1 && board[r+1][c] == color)
        count += (r < size-2 && board[r+2][c] == color)? 2 : 1;
    if (count > 2) return false;
    // check for triple in col
    count = 1;  
    if (c > 0 && board[r][c-1] == color)
        count += (c > 1 && board[r][c-2] == color)? 2 : 1;
    if (c < size-1 && board[r][c+1] == color)
        count += (c < size-2 && board[r][c+2] == color)? 2 : 1;
    return count <= 2;
}

/* Move class functions */

// Constructors/Destructor/Operators
Move::Move(Tile color, unsigned int row, unsigned int col)
    : color(color), row(row), col(col) {}
Move::Move(const Move &o): color(o.color), row(o.row), col(o.col) {}
Move::~Move() {}
