namespace Chess {
    public class BoardFunctions {
        // basically subtracting the letter a from it to get a number from it because ascii
        public static int[] StartSquareFromMoveName(string name) {
            return new int[] {name[1] - '1', name[0] - 'a'};
        }
        public static int[] TargetSquareFromMoveName(string name) {
            int g = name[2] - 'a';
            Console.WriteLine("Set Target To " + (int)name[2] + "-" + (int)'a' + "=" + g);
            return new int[] {name[3] - '1', name[2] - 'a'};
        }
        public static bool MoveIsPromotionFromName(string name) {
            return name.Length > 4;
        }
        public static Piece GetPromotionPiece(string name, Square target, Piece piece) {
            if(piece.type == PieceType.Pawn) {
                if(name.Length < 4) {
                    Console.Error.Write("Move is not a promotion");
                    return new Piece(false, PieceType.None, target.rank, target.file);
                } else {
                    if(name[5] == 'n') {
                        return new Piece(piece.isWhite, PieceType.Knight, target.rank, target.file);
                    } else if(name[5] == 'b') {
                        return new Piece(piece.isWhite, PieceType.Knight, target.rank, target.file);
                    } else if(name[5] == 'r') {
                        return new Piece(piece.isWhite, PieceType.Knight, target.rank, target.file);
                    } else if(name[5] == 'q') {
                        return new Piece(piece.isWhite, PieceType.Knight, target.rank, target.file);
                    } else {
                        Console.Error.Write("Piece To Promote To Is Invalid");
                        return new Piece(false, PieceType.None, target.rank, target.file);
                    }
                }
            } else {
                Console.Error.Write("Cannot Promote A Piece That Isn't A Pawn");
                return new Piece(false, PieceType.None, target.rank, target.file);
            }
        }
        public static void SwapSquares(ref Square[,] squares, int r1, int f1, int r2, int f2) {
            Square temp;
            temp = squares[r1,f1];
            squares[r1,f1] = squares[r2,f2];
            squares[r2,f2] = temp;
        }
    }
}