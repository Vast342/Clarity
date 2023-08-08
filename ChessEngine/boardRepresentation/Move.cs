namespace Chess {
    public class Move {
        public string name;
        public Square targetSquare;
        public Square startSquare;
        public Piece piece;
        public Piece capturedPiece;
        public Piece promotionPiece;
        public bool isCapture;
        public bool isPromotion;
        public bool isCastle;
        public int castleType;
        public bool isEnPassant;
        public Square enPassantSquare;
        public Move(string n, Board board) {
            name = n;
            // castling detection
            if(n == "e1g1" && board.castlingRights[0]) {
                isCastle = true;
                castleType = 1;
            }
            if(n == "e1b1" && board.castlingRights[1]) {
                isCastle = true;
                castleType = 2;
            }
            if(n == "e8g8" && board.castlingRights[2]) {
                isCastle = true;
                castleType = 3;
            }
            if(n == "e8b8" && board.castlingRights[3]) {
                isCastle = true;
                castleType = 4;
            }
            // everything else
            int[] sq = BoardFunctions.StartSquareFromMoveName(n);
            startSquare = board.GetSquareFromPosition(sq[0], sq[1]);
            piece = startSquare.piece;
            sq = BoardFunctions.TargetSquareFromMoveName(n);
            targetSquare = board.GetSquareFromPosition(sq[0], sq[1]);
            if(targetSquare.piece.type != PieceType.None) {
                isCapture = true;
                capturedPiece = targetSquare.piece;
            } else if(piece.IsPawn && startSquare.file != targetSquare.file) {
                // en passant detection
                isEnPassant = true;
                enPassantSquare = board.GetSquareFromPosition(targetSquare.rank - (piece.isWhite ? 1 : -1), targetSquare.file);
            }
            // promotion detection
            isPromotion = BoardFunctions.MoveIsPromotionFromName(n);
            if(isPromotion) {
                promotionPiece = BoardFunctions.GetPromotionPiece(n, targetSquare, piece);
            }
        }
        /*
        public Move(int index1, int index2, Board board) {
            startSquare = board.GetSquareFromIndex(index1);
            targetSquare = board.GetSquareFromIndex(index2);
            piece = startSquare.piece;
            if(targetSquare.piece.type != PieceType.None) {
                isCapture = true;
                capturedPiece = targetSquare.piece;
            }
        }
        public Move(int rank1, int file1, int rank2, int file2, Board board) {
            startSquare = board.GetSquareFromPosition(rank1, file1);
            targetSquare = board.GetSquareFromPosition(rank2, file2);
        }
        */
    }
}