#ifndef SOLVE_H_
#define SOLVE_H_

#include "game.hpp"

/* Handles input for setting up a game */
Game *generateGame();

/* Attempts to solve the game using known strategies
 *  - Returns true on success, false on failure
 */
bool solve(Game *game);

#endif  // SOLVE_H_
