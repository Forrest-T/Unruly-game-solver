#ifndef SOLVE_H_
#define SOLVE_H_

#include "game.hpp"

Game *generateGame();

bool solve(Game *game);
bool solve2(Game *game);

bool tryMove(Move *m, Game *game, int depth);

// vector of Move pointers
typedef std::vector<Move*> mvec;
void freevec(mvec *v);

#endif  // SOLVE_H_
