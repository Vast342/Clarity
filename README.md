# ChessEngineAgain
The chess engine from earlier, rewritten in C++

## Note: This repo is temporary and will be merged into Vast342/ChessEngine once the board representation is done and a basic bot is created.

### Note again, this currently uses the pext instruction from bmi2, which requires either an Intel CPU or a Zen 3 or newer CPU to work. If you are using a CPU that is neither of those, switch to using the classical approach move generation in movegen.cpp.

Special Thanks to:

  https://github.com/toanth Toanth: General help and explaining things I didn't understand before
  
  https://github.com/Ciekce Ciekce: Preventing the cardinal sins of C++ since day 1
  
  https://github.com/RedBedHed RedBedHed: Lookup tables for move generation
  
  No link yet JW: More random C++ things
  
  https://github.com/mcthouacbb A_Randomnoob: Helping with a lot of random engine bits

  https://github.com/zzzzz151 z5: Helped find a LOT of random issues. (like a lot)


THINGS TO ADD TO BOARD REPRESENTATION
  1. GetMovesQSearch(captures and checks only)
  2. zobrist hashing with incremental updates
  3. general performancey things
  4. Magic bitboards

General things:
  1. UCI support w/ custom commands (get-fen, make-move, perft, perft-suite, in-check, etc)
  2. A web version of it potentially?
  3. unleash it on Lichess (I feel bad for anyone who has to play against it)
