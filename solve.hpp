#ifndef SOLVE_H_
#define SOLVE_H_

#include <optional>
#include "game.hpp"

/* Handles input for setting up a game */
Game generateGame();

/* Attempts to solve the game using known strategies
 *  - Returns true on success, false on failure
 */
bool solve(Game &game);

/* Attempt to find a (maybe) valid move that can be inferred from the board state
 *  - optionally allows generating invalid moves
 */
std::optional<Move> directInference(Game &, bool allowInvalidMoves);
/*  */
std::optional<Move> recursiveInference(Game &, unsigned int maxDepth);

#endif  // SOLVE_H_
