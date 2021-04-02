#ifndef SOLVE_H_
#define SOLVE_H_

#include <optional>
#include "game.hpp"

/* Handles user input for setting up a game */
Game generateGame();

/* Attempts to solve the game using known strategies
 *  - Returns true on success, false on failure
 */
bool solve(Game &game);

/* Attempt to infer a move by elimination, optionally allowing invalid moves */
std::optional<Move> directInference(Game &, bool allowInvalidMoves);
/* Attempt to infer a move by speculatively playing ahead */
std::optional<Move> speculativeInference(Game &);

#endif  // SOLVE_H_
