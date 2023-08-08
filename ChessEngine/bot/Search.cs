using System;
using Chess;
public class Bot {
    public static void Main() {
        Console.WriteLine("IT'S ALIVE");
        //Console.WriteLine("No bot function at this time");
        Board board = new("rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 3");
        Move[] moves = board.GetLegalMoves();
        foreach(Move move in moves) {
            if(move.isEnPassant) board.MakeMove(move);
        }
        if(board.GetFenString() == "rnbqkbnr/ppp1p1pp/3P4/5p2/8/8/PPPP1PPP/RNBQKBNR b KQkq - 0 3") {
            Console.WriteLine("Test passed");
        } else {
            Console.WriteLine("Test failed, outputted " + board.GetFenString());
        }
    }
}