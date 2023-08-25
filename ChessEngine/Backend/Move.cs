namespace Chess {
    // I spent a decent while trying to replace this format but it's probably good for now
    // potential optimisations could include detecting if it's en passant, or writing that the move was en passant when it's made, and updating the  board that way instead of through the BoardState
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
        /// <summary>
        /// Converts the move to long algebraic form
        /// </summary>
        /// <returns>the long algebraic form</returns>
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
        /// <summary>
        /// Checks if the move is a capture on it's board
        /// </summary>
        /// <param name="board">The board the move is used on</param>
        /// <returns>is it a capture?</returns>
        public bool IsCapture(Board board) {
            return board.squares[endSquare] != Piece.None;
        }
        /// <summary>
        /// Converts the move to a number, to be used by the move table
        /// </summary>
        /// <returns></returns>
        public int ToNumber() {
            return (startSquare << 6) + endSquare;
        }
    }
}