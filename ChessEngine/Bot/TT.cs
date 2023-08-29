using Chess;

namespace Bot.Essentials {
    public struct TranspositionTable {
        public static ulong mask = 0xFFFFFF;
        public Transposition[] table;
        /// <summary>
        /// Creates a new Transposition Table
        /// </summary>
        public TranspositionTable() {
            table = new Transposition[mask + 1];
        }
        /// <summary>
        /// Clears the current transposition table
        /// </summary>
        public void Clear() {
            table = new Transposition[mask + 1];
        }
        /// <summary>
        /// Updates the best move saved in the table
        /// </summary>
        /// <param name="m">The new best move decided by the bot</param>
        /// <param name="z">The zobrist hash of the position</param>
        public void UpdateBestMove(Move m, ulong z) {
            table[z & mask].bestMove = m;
        }
        public void WriteEntry(Transposition entry, ulong z) {
            table[z & mask] = entry;
        }
        /// <summary>
        /// Updates the depth at a point in the table
        /// </summary>
        /// <param name="depth">The new depth</param>
        /// <param name="z">The zobrist hash of the position</param>
        public void UpdateDepth(int depth, ulong z) {
            table[z & mask].depth = depth;
        }
        /// <summary>
        /// Reads the best move for this position from the table
        /// </summary>
        /// <param name="z">The zobrist hash of the position</param>
        /// <returns>The best move from the table</returns>
        public Move ReadBestMove(ulong z) {
            return table[z & mask].bestMove;
        }
        /// <summary>
        /// Reads the depth searched to from the table
        /// </summary>
        /// <param name="z">The zobrist hash of the position</param>
        /// <returns>The depth from the table</returns>
        public int ReadDepth(ulong z) {
            return table[z & mask].depth;
        }
        public Transposition ReadEntry(ulong z) {
            return table[z & mask];
        }
        public int ReadScore(ulong z) {
            return table[z & mask].score;
        }
    }
    public struct Transposition {
        private const sbyte EXACT = 0, LOWERBOUND = -1, UPPERBOUND = 1, INVALID = -2;
        public Transposition(ulong zKey, Move m, int d, int s, sbyte f) {
            zobristKey = zKey;
            bestMove = m;
            depth = d;
            score = s;
            flag = f;
        }
        public ulong zobristKey;
        public Move bestMove;
        public int depth = 0, score = 0;
        public sbyte flag = INVALID;
    }
}