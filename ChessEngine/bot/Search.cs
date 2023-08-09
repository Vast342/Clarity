using System;
using Chess;
public class Bot {
    public static void Main() {
        Console.WriteLine("IT'S ALIVE");
        //Console.WriteLine("No bot function at this time");
        Board board = new("rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 3");
        Move[] moves = board.GetLegalMoves();
        
    }
}