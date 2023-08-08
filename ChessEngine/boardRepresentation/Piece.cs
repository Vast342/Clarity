using System;

namespace Chess {
    public struct Piece {
        public bool isWhite;
        public PieceType type;

        public readonly bool IsNull => type is PieceType.None;
        public readonly bool IsRook => type is PieceType.Rook;
        public readonly bool IsKnight => type is PieceType.Knight;
        public readonly bool IsBishop => type is PieceType.Bishop;
        public readonly bool IsQueen => type is PieceType.Queen;
        public readonly bool IsKing => type is PieceType.King;
        public readonly bool IsPawn => type is PieceType.Pawn;
        public int rank;
        public int file;
        public int index;
        public ulong legalMoves;
        public Piece(bool w, PieceType t, int r, int f) {
            isWhite = w;
            type = t;
            rank = r;
            file = f;
            index = 8*r + f;
        }
    }
}