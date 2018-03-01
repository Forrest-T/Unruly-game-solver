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
    // solve(game);
    cout << endl << "Solution Attempt:" << endl;
    solve2(game);
    game->print();
    if (game->verify()) {
        cout << endl << "Solution:" << endl;
        game->print();
        solved = true;
    }
    delete game;
}

bool solve(Game *game) {
    std::vector<Move*> *moves = game->generateMoves();
    for (int d = 1; d < 100 && !solved; d++) {
        cout << "Depth " << d << "..." << endl;
        for (auto &it : *moves) {
            if (solved) break;
            if (!tryMove(it, game, d)) {
                Move opp(*it);
                opp.color = (opp.color == B)? W:B;
                // if opposite move fails, unwinnable
                if (!tryMove(&opp, game, d)) {
                    freevec(moves);
                    return false;
                }
                // if opposite move was okay, use it
                game->apply(opp);
                // start over with the new information
                freevec(moves);
                moves = game->generateMoves();
                d = 1;
                break;
            }
        }
    }
    freevec(moves);
    return true;
}

bool solve2(Game *game) {
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

bool tryMove(Move *m, Game *game, int depth) {
    if (depth == 0 || solved) return true;
    game->apply(*m);
    stack<Move*> stk;
    while (Move *m = game->inferDirectly()) {
        game->apply(*m);
        stk.push(m);
    }
    mvec *moves = game->generateMoves();
    // return failure if the game is unwinnable
    if (moves == NULL) {
        while (!stk.empty()) {
            game->undo(*(stk.top()));
            delete stk.top();
            stk.pop();
        }
        game->undo(*m);
        return false;
    }
    if (moves->size() == 0 && game->verify()) {
        solved = true;
        game->print();
    }
    for (auto &it : *moves) {
        // if a move is invalid, the game is unwinnable
        if (!game->isValid(it->color, it->row, it->col)) {
            while (!stk.empty()) {
                game->undo(*(stk.top()));
                delete stk.top();
                stk.pop();
            }
            game->undo(*m);
            freevec(moves);
            return false;
        }
    }
    // all moves were okay, so try each one
    for (unsigned int it = 0; it < moves->size(); it++) {
        if (!tryMove((*moves)[it], game, depth-1)) {
            Move opp(*(*moves)[it]);
            opp.color = (opp.color == B)? W:B;
            // if one failed, check the opposite for an unwinnable game
            if (!tryMove(&opp, game, depth-1)) {
                while (!stk.empty()) {
                    game->undo(*(stk.top()));
                    delete stk.top();
                    stk.pop();
                }
                game->undo(*m);
                freevec(moves);
                return false;
            }
            // remove the move that failed
            delete (*moves)[it];
            moves->erase(moves->begin()+(--it));
        }
    }
    while (!stk.empty()) {
        game->undo(*(stk.top()));
        delete stk.top();
        stk.pop();
    }
    game->undo(*m);
    freevec(moves);
    return true;
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
    game->checkrep();
#endif
    delete[] tiles;
    return game;
}
