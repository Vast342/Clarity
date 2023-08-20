namespace Chess {
    // soon to be replaced by a 16 bit number
    // according to me earlier i'm thinking 16 bits, with a format like FFFFSSSSSSEEEEEE where F is a flag for if it's a capture or something, S is the start square's index, and E is the end square index
    public struct Move {
        public int startSquare;
        public int endSquare;
        public int promotionType;
        public BoardState state;
        /// <summary>
        /// Makes a new move with the starting and ending squares.
        /// </summary>
        /// <param name="start">Starting square of the move</param>
        /// <param name="end">Ending square of the move</param>
        /// <param name="pType">The promotion piece type, if any</param>
        public Move(int start, int end, int pType, BoardState s) {
            startSquare = start;
            endSquare = end;
            promotionType = pType;
            state = new(s);
        }
        public string ConvertToLongAlgebraic() {
            int startRank = startSquare >> 3;
            int startFile = startSquare & 7;
            int endRank = endSquare >> 3;
            int endFile = endSquare & 7;
            string name = "";
            name += (char)(startFile + 'a');
            name += startRank + 1;
            name += (char)(endFile + 'a');
            name += endRank + 1;
            return name;
        }
        public Move(string longAlgebraicForm, Board board) {
            char[] chars = longAlgebraicForm.ToCharArray();
            int startRank = chars[1] - '1';
            int startFile = chars[0] - 'a';
            int endRank = chars[3] - '1';
            int endFile = chars[2] - 'a';
            if(chars.Length > 4) {
                promotionType = chars[4];
            }
            startSquare = startRank * 8 + startFile;
            endSquare = endRank * 8 + endFile;

            state = new(board);
        }
        public bool IsCapture(Board board) {
            return board.squares[endSquare] != Piece.None;
        }
    }
}