/* Copyright 2018, Forrest Timour */

#include <assert.h>

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <vector>
#include <string>
#include <stack>

#include "game.hpp"
#include "solve.hpp"

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::stack;
using std::setw;

static bool solved = false;

int main() {
    Game *game = generateGame();
    cout << endl << "Current Game:" << endl;
    game->print();
    solved = false;
    // infer what moves it can
    while (Move *m = game->inferDirectly()) {
        game->apply(*m);
        delete m;
    }
    cout << endl << "Direct Inference:" << endl;
    game->print();
    solve(game);
    if (game->verify()) {
        cout << endl << "Solution:" << endl;
        game->print();
        solved = true;
    } else {
        cout << endl << "Solution Attempt:" << endl;
        game->print();
    }
    delete game;
}

bool solve(Game *game) {
    bool foundMove = true;
    Move *m;
    while (foundMove) {
        foundMove = false;
        // make direct inferences
        while ((m = game->inferDirectly())) {
            game->apply(*m);
            delete m;
            foundMove = true;
            continue;
        }
        if (!foundMove) {
            while ((m = game->infer1())) {
                game->apply(*m);
                delete m;
                foundMove = true;
                continue;
            }
        }
        // if (!foundMove)
        //   infer 1 of color left
        //   infer 2 of color left...
    }
    return false;
}

void freevec(mvec *v) {
    for (auto &m : *v)
        delete m;
    delete v;
}

Game *generateGame() {
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
    Game *game = new Game(size, tiles);
#ifdef DEBUG
    assert(game->checkrep());
#endif
    delete[] tiles;
    return game;
}
