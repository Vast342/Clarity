namespace Bot.Essentials {
    // this class is to be used with anything necessary for moves, like history heuristic.
    // that being said, it is currently unused.
    public struct MoveTable {
        public MoveTable() {
            table = new ushort[ushort.MaxValue];
        }
        ushort[] table = new ushort[ushort.MaxValue];
        /// <summary>
        /// Clears the Move Table
        /// </summary>
        public void Clear() {
            table = new ushort[ushort.MaxValue];
        }
        /// <summary>
        /// Adds a cutoff to the array at the position
        /// </summary>
        /// <param name="m">the position in question</param>
        public void AddCutoff(int m) {
            table[m]++;
        }
        /// <summary>
        /// Resets the value at an index
        /// </summary>
        /// <param name="m">the index in question</param>
        public void ResetCutoff(int m) {
            table[m] = 0;
        }
        /// <summary>
        /// reads the number of cutoffs at the position
        /// </summary>
        /// <param name="m"></param>
        /// <returns></returns>
        public int ReadCutoff(int m) {
            return table[m];
        }
    }
}