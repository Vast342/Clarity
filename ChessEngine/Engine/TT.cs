using Chess;

namespace Engine.Essentials {
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
        public void WriteZobrist(ulong z) {
            table[z & mask].zobristKey = z;
        }
        /// <summary>
        /// Reads the best move for this position from the table
        /// </summary>
        /// <param name="z">The zobrist hash of the position</param>
        /// <returns>The best move from the table</returns>
        public Move ReadBestMove(ulong z) {
            return table[z & mask].bestMove;
        }
        public Transposition ReadEntry(ulong z) {
            return table[z & mask];
        }
        public ulong ReadZobrist(ulong z) {
            return table[z & mask].zobristKey;
        }
    }
    // Very limited in scope currently
    public struct Transposition {
        public Transposition(Move m, ulong zKey){
            bestMove = m;
            zobristKey = zKey;
        }
        public ulong zobristKey;
        public Move bestMove;
    }
}