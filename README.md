# Clarity
---

*logo in progress*

The C++ rewrite is complete (not quite yet, but bear with me)! The engine is now faster and better, using new techniques, and I have learned a new language!

The engine has also received a name, and that is Clarity. 

#### Warning: Engine currently uses pext for move generation, which means that CPUs without BMI2 or CPUs with a slow implementation (Zen 2 or earlier) will not be able to use it, A replacement (magic bitboards) is underway.

### Special Thanks (in no particular order):

  https://github.com/toanth Toanth: General help and explaining things I didn't understand before
  
  https://github.com/Ciekce Ciekce: Preventing the cardinal sins of C++ since day 1
  
  https://github.com/RedBedHed RedBedHed: Lookup tables for move generation
  
  No link yet JW: More random C++ things
  
  https://github.com/mcthouacbb A_Randomnoob: Helping with a lot of random engine bits

### Todo lists:

Board Representation:
  1. Zobrist hashing with incremental updates
  2. General performancey things
  3. Magic bitboards as a backup for non-bmi2 CPUs

Move Ordering:
  1. Transposition table best move
  2. History

Search:
  1. Transposition Table & pruning
  2. Extensions
  3. NMP
  4. LMR

Evaluation:
  1. Passed pawns
  2. Pawn structure
  3. More if I think of it
