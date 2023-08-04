using System;
using Chess;
public class Bot {
    public static void Main() {
        Console.WriteLine("IT'S ALIVE");
        Board board = new Board("r6k/pp2r2p/4Rp1Q/3p4/8/1N1P2R1/PqP2bPP/7K b - - 0 24");
        // initializes the table used for zobrist hash generation later.
        board.InitializeZobrist();
        Move move = new Move("f2g3", board);
        board.MakeMove(move);
        Console.WriteLine(board.GetFenString());
        /*board = new Board("5rk1/1p3ppp/pq3b2/8/8/1P1Q1N2/P4PPP/3R2K1 w - - 2 27");
        move = new Move("f3g5", board);
        board.MakeMove(move);
        board = new Board("r2qr1k1/b1p2ppp/pp4n1/P1P1p3/4P1n1/B2P2Pb/3NBP1P/RN1QR1K1 b - - 1 16");
        move = new Move("d8d4", board);
        board.MakeMove(move);
        board = new Board("8/8/4k1p1/2KpP2p/5PP1/8/8/8 w - - 0 53");
        move = new Move("c5d4", board);
        board.MakeMove(move);
        board = new Board("4r3/1k6/pp3r2/1b2P2p/3R1p2/P1R2P2/1P4PP/6K1 w - - 0 35");
        move = new Move("h2h4", board);
        board.MakeMove(move); */
    }
}