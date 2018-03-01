/* Copyright 2018, Forrest Timour */

#include <assert.h>

#include <stack>
#include <vector>
#include <iostream>
#include <algorithm>

#include "game.hpp"
#include "solve.hpp"

/* Game class functions */

Game::Game(unsigned int size, Tile state[]): size(size) {
    numTiles = 0;
    Tile temp;
    rowB = new unsigned int[size];
    colB = new unsigned int[size];
    rowW = new unsigned int[size];
    colW = new unsigned int[size];
    for (unsigned int i = 0; i < size; i++) {
        rowB[i] = 0;
        colB[i] = 0;
        rowW[i] = 0;
        colW[i] = 0;
    }
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
Game::Game(const Game &o): numTiles(o.numTiles), size(o.size) {
    rowB = new unsigned int[size];
    colB = new unsigned int[size];
    rowW = new unsigned int[size];
    colW = new unsigned int[size];
    board = new Tile*[size];
    for (unsigned int i = 0; i < size; i++) {
        board[i] = new Tile[size];
        for (unsigned int j = 0; j < size; j++)
            board[i][j] = o.board[i][j];
        rowB[i] = o.rowB[i];
        colB[i] = o.colB[i];
        rowW[i] = o.rowW[i];
        colW[i] = o.colW[i];
    }
}
Game::~Game() {
    for (unsigned int i = 0; i < size; i++)
        delete[] board[i];
    delete[] board;
    delete[] rowB;
    delete[] colB;
    delete[] rowW;
    delete[] colW;
}

bool Game::verify() {
    return numTiles == size*size && checkrep();
}

Move *Game::inferDirectly() {
    // when there is only one valid move
    for (unsigned int r = 0; r < size; r++) {
        for (unsigned int c = 0; c < size; c++) {
            if (board[r][c] != E)
                continue;
            if (!isValid(B, r, c) && isValid(W, r, c)) {
                return new Move(W, r, c, this);
            }
            if (!isValid(W, r, c) && isValid(B, r, c))
                return new Move(B, r, c, this);
        }
    }
    return NULL;
}

Move *Game::infer1() {
    // try each position, looking for ones that don't work
    for (unsigned int r = 0; r < size; r++) {
        if (rowB[r] == size/2-1) {
            for (unsigned int c = 0; c < size; c++)
                if (board[r][c] == E && !attempt(Move(B, r, c, this)))
                    return new Move(W, r, c, this);
        } else if (rowW[r] == size/2-1) {
            for (unsigned int c = 0; c < size; c++)
                if (board[r][c] == E && !attempt(Move(W, r, c, this)))
                    return new Move(B, r, c, this);
        }
    }
    for (unsigned int c = 0; c < size; c++) {
        if (colB[c] == size/2-1) {
            for (unsigned int r = 0; r < size; r++)
                if (board[r][c] == E && !attempt(Move(B, r, c, this)))
                    return new Move(W, r, c, this);
        } else if (colW[c] == size/2-1) {
            for (unsigned int r = 0; r < size; r++)
                if (board[r][c] == E && !attempt(Move(W, r, c, this)))
                    return new Move(B, r, c, this);
        }
    }
    return NULL;
}

bool Game::attempt(Move move) {
    apply(move);
    std::stack<Move*> stk;
    while (Move *m = inferDirectly()) {
        apply(*m);
        stk.push(m);
    }
    bool winnable = isWinnable();
    while (!stk.empty()) {
        undo(*(stk.top()));
        delete stk.top();
        stk.pop();
    }
    undo(move);
    return winnable;
}

void Game::print() {
    std::cout << std::endl;
    for (unsigned int r = 0; r < size; r++) {
        std::cout << " ";
        for (unsigned int c = 0; c < size; c++) {
            if (board[r][c] == B)
                std::cout << "b";
            else if (board[r][c] == W)
                std::cout << "w";
            else
                std::cout << "-";
        }
        std::cout << std::endl;
    }
}

void Game::apply(const Move &m) {
#ifdef DEBUG
    assert(isValid(m.color, m.row, m.col));
#endif
    board[m.row][m.col] = m.color;
    if (m.color == B) {
        rowB[m.row]++;
        colB[m.col]++;
    } else {
        rowW[m.row]++;
        colW[m.col]++;
    }
    numTiles++;
#ifdef DEBUG
    assert(checkrep());
#endif
}

void Game::undo(const Move &m) {
#ifdef DEBUG
    assert(m.color == board[m.row][m.col]);
#endif
    board[m.row][m.col] = E;
    if (m.color == B) {
        rowB[m.row]--;
        colB[m.col]--;
    } else {
        rowW[m.row]--;
        colW[m.col]--;
    }
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

std::vector<Move*> *Game::generateMoves() {
    std::vector<Move*> *moves = new std::vector<Move*>();
    moves->reserve(2*(size*size-numTiles));
    for (unsigned int r = 0; r < size; r++) {
        for (unsigned int c = 0; c < size; c++) {
            if (board[r][c] != E)
                continue;
            if (isValid(B, r, c))
                moves->push_back(new Move(B, r, c, this));
            if (isValid(W, r, c))
                moves->push_back(new Move(W, r, c, this));
        }
    }
    std::sort(moves->begin(), moves->end());
    return moves;
}

bool Game::isWinnable() {
    for (unsigned int r = 0; r < size; r++)
        for (unsigned int c = 0; c < size; c++)
            if (board[r][c] == E && !isValid(W, r, c) && !isValid(B, r, c))
                return false;
    return true;
}

bool Game::isWinnable(unsigned int row, unsigned int col) {
    for (unsigned int r = 0; r < size; r++)
        if (board[r][col] == E && !isValid(W, r, col) && !isValid(B, r, col))
            return false;
    for (unsigned int c = 0; c < size; c++)
        if (board[row][c] == E && !isValid(W, row, c) && !isValid(B, row, c))
            return false;
    return true;
}

bool Game::isValid(Tile color, unsigned int r, unsigned int c) {
    unsigned int count = 1;
    if (color == B) {
        // check for row and col saturation
        if (rowB[r] == size/2 || colB[c] == size/2)
            return false;
        count = 1;  // check for triple in row
        if (r > 0 && board[r-1][c] == B)
            count += (r > 1 && board[r-2][c] == B)?2:1;
        if (r < size-1 && board[r+1][c] == B)
            count += (r < size-2 && board[r+2][c] == B)?2:1;
        if (count > 2)
            return false;
        count = 1;  // check for triple in col
        if (c > 0 && board[r][c-1] == B)
            count += (c > 1 && board[r][c-2] == B)?2:1;
        if (c < size-1 && board[r][c+1] == B)
            count += (c < size-2 && board[r][c+2] == B)?2:1;
        return count <= 2;
    } else if (color == W) {
        // check for row and col saturation
        if (rowW[r] == size/2 || colW[c] == size/2)
            return false;
        count = 1;  // check for triple in row
        if (r > 0 && board[r-1][c] == W)
            count += (r > 1 && board[r-2][c] == W)?2:1;
        if (r < size-1 && board[r+1][c] == W)
            count += (r < size-2 && board[r+2][c] == W)?2:1;
        if (count > 2)
            return false;
        count = 1;  // check for triple in col
        if (c > 0 && board[r][c-1] == W)
            count += (c > 1 && board[r][c-2] == W)?2:1;
        if (c < size-1 && board[r][c+1] == W)
            count += (c < size-2 && board[r][c+2] == W)?2:1;
        return count <= 2;
    }
    return false;
}

/* Move class functions */

// Constructors/Destructor/Operators
Move::Move(Tile color, unsigned int row, unsigned int col, Game *g)
          : color(color), row(row), col(col), g(g) {}
Move::Move(const Move &o): color(o.color), row(o.row), col(o.col), g(o.g) {}
Move::~Move() {}
bool Move::operator=(const Move &o) {
    return color == o.color && row == o.row && col == o.col && g == o.g;
}
