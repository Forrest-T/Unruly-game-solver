#ifndef GAME_H_
#define GAME_H_

#include <stdint.h>
#include <vector>

typedef enum tileState {B=0, W=1, E=2} Tile;
class Move;

class Game {
  public:
    // The current state of the game board
    unsigned int numTiles;
    unsigned int size;
    Tile **board;
    // The the number of colored tiles in each row and col
    std::vector<unsigned int> rowB;
    std::vector<unsigned int> colB;
    std::vector<unsigned int> rowW;
    std::vector<unsigned int> colW;

    // Constructors/Destructor
    Game(unsigned int size, Tile state[]);
    Game(const Game &other);
    ~Game();

    friend std::ostream & operator<<(std::ostream &os, Game const &game);

    /* Verifies that the game has been solved */
    bool verify();

    /* Attempts to infer a single tile from the board's
     * current position, using a deterministic set of rules.
     * - Return: a Move pointer that must be freed, or NULL
     */
    Move *inferDirectly();
    Move *infer1();

    /* Returns a sorted vector of valid moves to try
     * - Return: sorted vector of moves
     * - Caller is responsible for freeing vector
     * [Currently Unused]
     */
    std::vector<Move*> *generateMoves();

    /* Checks if a move is valid */
    bool isValid(Tile color, unsigned int row, unsigned int col);

    /* Applies a move to the board. Does not check for validity */
    void apply(const Move &m);

    /* Undies an applied move to the board. */
    void undo(const Move &m);

  private:
    /* Ensures that every non-empty tile has at least one valid move
     *  - overloaded if only one row/column needs to be checked
     */
    bool isWinnable();
    bool isWinnable(unsigned int row, unsigned int col);

    /* Attempts a move and determines using direct inference whether
     * the game appears to be in a solvable state.
     * - returns false if the game is unwinnable
     */
    bool attempt(Move m);

    /* Used for debugging. Checks internal representation */
    bool checkrep();
};

std::ostream & operator<<(std::ostream &, Game const &);

class Move {
  public:
    // The tile and location
    Tile color;
    unsigned int row;
    unsigned int col;
    Game *g;

    Move(Tile color, unsigned int row, unsigned int col, Game *g);
    Move(const Move &o);
    ~Move();
};
 
#endif  // GAME_H_
