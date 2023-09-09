using Chess;

namespace Engine.Essentials {
    /*public struct TranspositionTable {
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
        public void WriteFlag(byte flag, ulong z) {
            table[z & mask].flag = flag;
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
        public byte ReadFlag(ulong z) {
            return table[z & mask].flag;
        }
        public bool IsEntryEqual(ulong z) {
            return table[z & mask].zobristKey == z;
        }
    }*/
    // Very limited in scope currently
    public struct Transposition {
        public Transposition(ulong z, Move m, byte f, int d){
            zobristKey = z;
            bestMove = m;
            flag = f;
            depth = d;
        }
        public ulong zobristKey;
        public Move bestMove;
        public byte flag = 4;
        public int depth;
    }
}