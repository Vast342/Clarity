# Clarity

The C++ rewrite is complete! Clarity is now faster and better, using new techniques, and I have learned a new language!

For those who didn't see the engine in its depressing, confusing, error-filled C# state, this engine was originally written in C# before I decided to switch to C++. That decision was one of the greatest I think I've ever had, as the engine is now faster, easier to read, less error-filled(hopefully), and better overall.

My current Estimate of elo (calculated by playing large sums of games against engines with known CCRL ratings) is 1800.

#### Warning: Clarity currently uses pext for move generation, which means that CPUs without BMI2 or CPUs with a slow implementation (Zen 2 or earlier) will not be able to use it, A replacement (magic bitboards) is underway.

### Special Thanks (in no particular order):

  [Toanth](https://github.com/toanth): General help and explaining things I didn't understand before
  
  [Ciekce](https://github.com/Ciekce): Preventing the cardinal sins of C++ since day 1
  
  [RedBedHed](https://github.com/RedBedHed): Lookup tables for move generation
  
  [JW](https://github.com/jw1912): More random C++ things
  
  [A_randomnoob](https://github.com/mcthouacbb): Helping with a lot of random engine bits

  [zzzzz](https://github.com/zzzzz151/): Ideas, planning, and a lot that I probably forgot

### Todo lists:

General Things:
  1. Web version of the engine
  2. or unleash it on lichess

Board Representation:
  1. General performancey things
  2. Magic bitboards as a backup for non-bmi2 CPUs

Search:
  1. NMP
  2. I am working on the list of ideas

Evaluation:
  1. Passed pawns
  2. Pawn structure
  3. Texel Tuning
  4. NNUE eventually


## Feature List:

Board Representation:
    1. Copymake moves
    2. Board represented using 8 bitboards
    3. Pext bitboards, lookups, and setwise move generation
    4. Repetition detection
    5. Incremental Zobrist hashing
    6. Incremental PSQT updates

Search: 
    1. PVS search:
        1. Aspiration windows
        2. Transposition Table(TT) cutoffs
        3. Reverse Futility Pruning
        4. Null Move Pruning
        5. Late Move Reductions (log function, generated on startup)
        6. Fail-soft
        7. Currently nonfunctional LMP
    2. Qsearch
        1. Failsoft
        2. TT cutoffs

Evaluation:
    1. PeSTO PSQTs and piece weights (soon to be tuned)
    2. Non-functional passed pawn detection

Move ordering:
    1. TT best move
    2. MVV-LVA
    3. History Heuristic
