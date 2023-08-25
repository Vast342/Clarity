using Chess;

namespace Bot.Essentials {
    public class TranspositionTable {
        public static ulong mask = 0x7FFFFF;
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
    }
    public class Transposition {
        public Move bestMove;
        public int depth;
    }
}