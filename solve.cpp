/* Copyright 2018, Forrest Timour */

#include <cassert>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <vector>
#include <string>

#include "game.hpp"
#include "solve.hpp"

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::setw;

int main() {
    Game game = generateGame();
    cout << endl << "Current Game:" << endl;
    std::cout << game;
    // infer what moves it can
    while (Move *m = game.inferDirectly()) {
        game.apply(*m);
        delete m;
    }
    cout << endl << "Direct Inference:" << endl;
    std::cout << game;
    if (solve(game)) {
        cout << endl << "Solution:" << endl;
    } else {
        cout << endl << "Solution Attempt:" << endl;
    }
    std::cout << game;
}

bool solve(Game &game) {
    Move *m;
    bool foundMove = true;
    // search until no moves are found
    while (foundMove) {
        foundMove = false;
        // prioritize making direct inferences
        while ((m = game.inferDirectly())) {
            game.apply(*m);
            delete m;
            foundMove = true;
            break;
        }
        // otherwise, try to infer with recursion depth 1
        if (!foundMove) {
            while ((m = game.infer1())) {
                game.apply(*m);
                delete m;
                foundMove = true;
                break;
            }
        }
    }
    return game.verify();
}

Game generateGame() {
    unsigned int size;
    cout << "Enter the board size: ";
    cin >> size;
    assert(!cin.fail());
    assert(size%2 == 0);
    string state(size*size, ' '), line;
    Tile *tiles = new Tile[size*size];
    cout << "enter 'w' for white, 'b' for black, - for empty" << endl;
    for (unsigned int i = 0; i < size; i++) {
        cout << "Enter row " << setw(2) << i+1 << ": ";
        cin >> line;
        while (line.size() != size) {
            cout << "Incorrect Size!\nEnter row " << setw(2) << i+1 << ": ";
            cin >> line;
        }
        for (unsigned int j = 0; j < size; j++)
            tiles[size*i+j] = (line[j] == 'w')? W: (line[j] == 'b')? B: E;
    }
    Game game(size, tiles);
#ifdef DEBUG
    assert(game->checkrep());
#endif
    delete[] tiles;
    return game;
}
