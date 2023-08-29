# ChessEngine
---

This project is my attempt at making a chess engine, the goal is to make it a powerful opponent, efficient, and most importantly, UCI compatible.

Currently, it is only the last of those 3.

It is also my attempt to learn development in raw c# after using it with Unity for so long.

Currently, I have a basic testing bot (negamax + ab pruning + mvv-lva ordering + material eval), some sketchy UCI support, and a few custom commands. 

Those commands are:

test: performs a specific group of tests. accepts board-rep, and outlier tests. These tests are made by me, specifically for whatever I am trying to fix or test at the time. As part of move-gen testing, it runs perft 2, 3, and 4 from the start position, and doesn't currently pass those.

get-fen: Outputs the current position of the board as a fen string, just to check if a move was made correctly.

perft [depth] [fen/startpos] [split/ ]: The most complicated one. It can perform a perft test at various depths and from various positions. depth is the depth of the perft test, the second field is either the fen string of the position or the word startpos, and if the last field is the word split it will divide the result by move, otherwise, it can be left blank for the total nodes explored.

in-check [fen]: outputs if the board state read from the fen string is in check or not.

make-move [move]: Makes the move on the board, and outputs the resulting fen. move must be in long algebraic notation.

perft-suite [suite]: Performs a suite of perft tests, and outputs specified results, followed by a total effect. Currently, due to performance limitations, it will restrict the tests performed to be under a certain number of total nodes. The only currently supported suite is known as Ethereal and is a series of tests from the bot Ethereal.
