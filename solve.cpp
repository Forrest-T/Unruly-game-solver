/* Copyright 2018, Forrest Timour */

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <optional>
#include <stack>
#include <string>
#include <vector>

#include "game.hpp"
#include "solve.hpp"

using std::cin;
using std::cout;
using std::cerr;
using std::endl;

int main() {
    Game game = generateGame();
    cout << endl << "Current Game:" << endl << game;
    if (solve(game)) {
        cout << endl << "Solution:" << endl;
    } else {
        cout << endl << "Solution Attempt:" << endl;
    }
    cout << game;
}

bool
solve(Game &game) {
    bool foundMove = true;
    // search until no moves are found
    while (foundMove) {
        foundMove = false;
        // prioritize making direct inferences
        while (auto m = directInference(game, false)) {
            game.apply(m.value());
            foundMove = true;
        }
        if (foundMove) continue;
        // otherwise, try to infer with recursion depth 1
        while (auto m = recursiveInference(game, 1)) {
            game.apply(m.value());
            foundMove = true;
            break;
        }
    }
    return game.verifySolved();
}

std::optional<Move>
directInference(Game &game, bool allowInvalidMoves) {
    for (unsigned int r = 0; r < game.size; r++) {
        for (unsigned int c = 0; c < game.size; c++) {
            if (game.board[r][c] != E) continue;
            if (!game.isValid(B, r, c) && (allowInvalidMoves || game.isValid(W, r, c)))
                return Move(W, r, c);
            if (!game.isValid(W, r, c) && (allowInvalidMoves || game.isValid(B, r, c)))
                return Move(B, r, c);
        }
    }
    return std::nullopt;
}

std::optional<Move>
recursiveInference(Game &game, unsigned int maxDepth) {
    if (!maxDepth) return std::nullopt;
    for (unsigned int r = 0; r < game.size; r++) {
        for (unsigned int c = 0; c < game.size; c++) {
            if (game.board[r][c] != E) continue;
            static constexpr Tile tiles[2] = { W, B };
            for (unsigned i : { 0, 1 }) {
                Move move(tiles[i], r, c);
                game.apply(move);
                std::stack<Move> moves;
                moves.push(move);
                while (auto m = directInference(game, true)) {
                    game.apply(m.value());
                    moves.push(m.value());
                }
                bool winnable = game.isWinnable();
                bool solved = game.verifySolved();
                while (!moves.empty()) {
                    game.undo(moves.top());
                    moves.pop();
                }
                if (solved) return move;
                if (!winnable) return Move(tiles[1-i], r, c);
            }
        }
    }
    return std::nullopt;
}

Game
generateGame() {
    unsigned int size;
    cout << "Enter the board size: ";
    cin >> size;
    assert(!cin.fail());
    assert(size % 2 == 0);
    std::string state(size*size, ' '), line;
    Tile *tiles = new Tile[size*size];
    cout << "enter 'w' for white, 'b' for black, - for empty" << endl;
    for (unsigned int i = 0; i < size; i++) {
        cout << "Enter row " << std::setw(2) << i+1 << ": ";
        cin >> line;
        while (line.size() != size) {
            cout << "Incorrect Size!\nEnter row " << std::setw(2) << i+1 << ": ";
            cin >> line;
        }
        for (unsigned int j = 0; j < size; j++)
            tiles[size*i+j] = (line[j] == 'w')? W: (line[j] == 'b')? B: E;
    }
    Game game(size, tiles);
#ifdef DEBUG
    assert(game.checkrep());
#endif
    delete[] tiles;
    return game;
}
