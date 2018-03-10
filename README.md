# Unruly-game-solver

One of my good friends introduced me to a puzzle-solving app for Android called simply _Puzzles_. This is my attempt to create a time-efficient solver for one of the games _Unruly_, without resorting to brute-force.

## Brief overview of the game:
* You are given a square board with an even number of rows/columns
* The board is partially filled with black and white tiles
* You must fill the board according to the following rules:
  * No three tiles of a given color may exist subsequently in a horizontal or vertical line
  * Every row and column must have the same number of black and white tiles

## Strategy
Two methods are implemented: inferDirectly() and infer1()
* inferDirectly() uses the idea that every square must be either black, or white. If only one color leads to an invalid game configuration, that color can be inferred.
* infer1() uses the idea that every row must contain half as many tiles of each color as the side length. If a row or column needs one more tile of a given color, each configuration will be tried. If this leads to an invalid configuration, the other color can be inferred.

## Notes
Originally, it was planned to extend infer1() to an arbitrary depth and number of remaining tiles. However, just these two methods are able to solve test games of size 14 completely. Termination is guaranteed, but there may exist some configurations which cannot be resolved to a solution.
