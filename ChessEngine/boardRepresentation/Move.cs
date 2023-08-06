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
        // true is right, false is left.
        public bool enPassantDirection;
        public Move(string n, Board board) {
            name = n;
            // castling detection
            if(n == "e1g1" && board.castlingRights[0] && board.GetSquareFromPosition(0,5).piece.type == PieceType.None && board.GetSquareFromPosition(0,6).piece.type == PieceType.None) {
                isCastle = true;
                castleType = 1;
            }
            if(n == "e1b1" && board.castlingRights[1] && board.GetSquareFromPosition(0,1).piece.type == PieceType.None && board.GetSquareFromPosition(0,2).piece.type == PieceType.None && board.GetSquareFromPosition(0,3).piece.type == PieceType.None) {
                isCastle = true;
                castleType = 2;
            }
            if(n == "e8g8" && board.castlingRights[2] && board.GetSquareFromPosition(7,5).piece.type == PieceType.None && board.GetSquareFromPosition(7,6).piece.type == PieceType.None) {
                isCastle = true;
                castleType = 3;
            }
            if(n == "e8b8" && board.castlingRights[3] && board.GetSquareFromPosition(7,1).piece.type == PieceType.None && board.GetSquareFromPosition(7,2).piece.type == PieceType.None && board.GetSquareFromPosition(7,3).piece.type == PieceType.None) {
                isCastle = true;
                castleType = 4;
            }
            // en passant detection

            // everything else
            int[] sq = BoardFunctions.StartSquareFromMoveName(n);
            startSquare = board.GetSquareFromPosition(sq[0], sq[1]);
            piece = startSquare.piece;
            sq = BoardFunctions.TargetSquareFromMoveName(n);
            targetSquare = board.GetSquareFromPosition(sq[0], sq[1]);
            if(targetSquare.piece.type != PieceType.None) {
                isCapture = true;
                capturedPiece = targetSquare.piece;
            }
            isPromotion = BoardFunctions.MoveIsPromotionFromName(n);
            if(isPromotion) {
                promotionPiece = BoardFunctions.GetPromotionPiece(n, targetSquare, piece);
            }
        }
    }
}