# Clarity
---

*logo in progress*

The C++ rewrite is complete! Clarity is now faster and better, using new techniques, and I have learned a new language!

For those who didn't see the engine in its depressing, confusing, error-filled C# state, this engine was originally written in C# before I decided to switch to C++. That decision was one of the greatest I think I've ever had, as the engine is now faster, easier to read, less error-filled(hopefully), and better overall.

My current Estimate of elo (calculated by playing large sums of games against engines with known CCRL ratings) is 1800.

#### Warning: Clarity currently uses pext for move generation, which means that CPUs without BMI2 or CPUs with a slow implementation (Zen 2 or earlier) will not be able to use it, A replacement (magic bitboards) is underway.

### Special Thanks (in no particular order):

  https://github.com/toanth Toanth: General help and explaining things I didn't understand before
  
  https://github.com/Ciekce Ciekce: Preventing the cardinal sins of C++ since day 1
  
  https://github.com/RedBedHed RedBedHed: Lookup tables for move generation
  
  https://github.com/jw1912 JW: More random C++ things
  
  https://github.com/mcthouacbb A_Randomnoob: Helping with a lot of random engine bits

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