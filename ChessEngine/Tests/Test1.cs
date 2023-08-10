using Chess;

public class Test1 {
    public static void Main() {
        Console.WriteLine("IT'S ALIVE!!!");
        Board board = new("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        List<Move> moves = board.GetLegalMoves();
        if(moves.Count == 20) {
            Console.WriteLine("Move count test passed");
        } else {
            Console.WriteLine("Move count test Failed, outputted " + moves.Count + " legal moves");
        }
        // everything
        if(board.occupiedBitboard == 18446462598732906495) {
            Console.WriteLine("Occupied bitboard test passed");
        } else {
            Console.WriteLine("Occupied bitboard test failed, outputted " + board.occupiedBitboard);
        }
        // pawns
        if(board.coloredPieceBitboards[1, 0] == 0b1111111100000000) {
            Console.WriteLine("White pawn bitboard test passed");
        } else {
            Console.WriteLine("White pawn bitboard test failed, outputted " + board.coloredPieceBitboards[1,0]);
        }
        if(board.coloredPieceBitboards[0, 0] == 0b0000000011111111000000000000000000000000000000000000000000000000) {
            Console.WriteLine("Black pawn bitboard test passed");
        } else {
            Console.WriteLine("Black pawn bitboard test failed, outputted " + board.coloredPieceBitboards[0,0]);
        }
        // knights
        if(board.coloredPieceBitboards[1, 1] == 0b01000010) {
            Console.WriteLine("White knight bitboard test passed");
        } else {
            Console.WriteLine("White knight bitboard test failed, outputted " + board.coloredPieceBitboards[1,1]);
        }
        if(board.coloredPieceBitboards[0, 1] == 0b0100001000000000000000000000000000000000000000000000000000000000) {
            Console.WriteLine("Black knight bitboard test passed");
        } else {
            Console.WriteLine("Black knight bitboard test failed, outputted " + board.coloredPieceBitboards[0,1]);
        }
        // bishops
        if(board.coloredPieceBitboards[1, 2] == 0b00100100) {
            Console.WriteLine("White bishop bitboard test passed");
        } else {
            Console.WriteLine("White bishop bitboard test failed, outputted " + board.coloredPieceBitboards[1,2]);
        }
        if(board.coloredPieceBitboards[0, 2] == 0b0010010000000000000000000000000000000000000000000000000000000000) {
            Console.WriteLine("Black bishop bitboard test passed");
        } else {
            Console.WriteLine("Black bishop bitboard test failed, outputted " + board.coloredPieceBitboards[0,2]);
        }
        // rooks
        if(board.coloredPieceBitboards[1, 3] == 0b10000001) {
            Console.WriteLine("White rook bitboard test passed");
        } else {
            Console.WriteLine("White rook bitboard test failed, outputted " + board.coloredPieceBitboards[1,3]);
        }
        if(board.coloredPieceBitboards[0, 3] == 0b1000000100000000000000000000000000000000000000000000000000000000) {
            Console.WriteLine("Black rook bitboard test passed");
        } else {
            Console.WriteLine("Black rook bitboard test failed, outputted " + board.coloredPieceBitboards[0,3]);
        }
        // queens
        if(board.coloredPieceBitboards[1, 4] == 0b00001000) {
            Console.WriteLine("White queen bitboard test passed");
        } else {
            Console.WriteLine("White queen bitboard test failed, outputted " + board.coloredPieceBitboards[1,4]);
        }
        if(board.coloredPieceBitboards[0, 4] == 0b0000100000000000000000000000000000000000000000000000000000000000) {
            Console.WriteLine("Black queen bitboard test passed");
        } else {
            Console.WriteLine("Black queen bitboard test failed, outputted " + board.coloredPieceBitboards[0,4]);
        }
        // kings
        if(board.coloredPieceBitboards[1, 5] == 0b00010000) {
            Console.WriteLine("White king bitboard test passed");
        } else {
            Console.WriteLine("White king bitboard test failed, outputted " + board.coloredPieceBitboards[1,5]);
        }
        if(board.coloredPieceBitboards[0, 5] == 0b0001000000000000000000000000000000000000000000000000000000000000) {
            Console.WriteLine("Black king bitboard test passed");
        } else {
            Console.WriteLine("Black king bitboard test failed, outputted " + board.coloredPieceBitboards[0,5]);
        }
        if(board.GetFenString() == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {
            Console.WriteLine("Fen encoder test passed");
        } else {
            Console.WriteLine("Fen encoder test failed, outputted " + board.GetFenString());
        }
    }
}