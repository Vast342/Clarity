using System;
using Chess;
public class Bot {
    public static void Main() {
        Console.WriteLine("IT'S ALIVE");
        Board board = new Board("r6k/pp2r2p/4Rp1Q/3p4/8/1N1P2b1/PqP3PP/7K w - - 0 1");
        // initializes the table used for zobrist hash generation later.
        board.InitializeZobrist();
        Move move = new Move("e6e7", board);
        board.MakeMove(move);
        if(board.GetFenString() == "r6k/pp2R2p/5p1Q/3p4/8/1N1P2b1/PqP3PP/7K b - - 0 2") {
            Console.WriteLine("Test 1 Success");
        } else {
            Console.WriteLine("Test 1 Failed, outputed " + board.GetFenString());
        }
        Board board2 = new Board("5rk1/1p3ppp/pq3b2/8/8/1P1Q1N2/P4PPP/3R2K1 w - - 0 27");
        Move move2 = new Move("f3g5", board2);
        board2.MakeMove(move2);
        if(board2.GetFenString() == "5rk1/1p3ppp/pq3b2/6N1/8/1P1Q4/P4PPP/3R2K1 b - - 0 28") {
            Console.WriteLine("Test 2 Success");
        } else {
            Console.WriteLine("Test 2 Failed, outputed " + board2.GetFenString());
        }
        Board board3 = new Board("r2qr1k1/b1p2ppp/pp4n1/P1P1p3/4P1n1/B2P2Pb/3NBP1P/RN1QR1K1 b - - 0 16");
        Move move3 = new Move("d8d4", board3);
        board3.MakeMove(move3);
        if(board3.GetFenString() == "r3r1k1/b1p2ppp/pp4n1/P1P1p3/3qP1n1/B2P2Pb/3NBP1P/RN1QR1K1 w - - 0 17") {
            Console.WriteLine("Test 3 Success");
        } else {
            Console.WriteLine("Test 3 Failed, outputed " + board3.GetFenString());
        }
        Board board4 = new Board("8/8/4k1p1/2KpP2p/5PP1/8/8/8 w - - 0 53");
        Move move4 = new Move("c5d4", board4);
        board4.MakeMove(move4);
        if(board4.GetFenString() == "8/8/4k1p1/3pP2p/3K1PP1/8/8/8 b - - 0 54") {
            Console.WriteLine("Test 4 Success");
        } else {
            Console.WriteLine("Test 4 Failed, outputed " + board4.GetFenString());
        }
        Board board5 = new Board("4r3/1k6/pp3r2/1b2P2p/3R1p2/P1R2P2/1P4PP/6K1 w - - 0 35");
        Move move5 = new Move("h2h4", board5);
        board5.MakeMove(move5);
        if(board5.GetFenString() == "4r3/1k6/pp3r2/1b2P2p/3R1p1P/P1R2P2/1P4P1/6K1 b - - 0 36") {
            Console.WriteLine("Test 5 Success");
        } else {
            Console.WriteLine("Test 5 Failed, outputed " + board5.GetFenString());
        }
    }
}