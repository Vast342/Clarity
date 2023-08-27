using System.Diagnostics;

namespace Chess {
    public class PerftTest {
        public PerftTest(string fen, int d, long e) {
            board = new Board(fen);
            depth = d;
            expectedResult = e;
        }
        public Board board;
        public int depth;
        public long expectedResult;
    }
    public class Perft {
        public static void PerformTestSuite(PerftTest[] tests) {
            int i = 0;
            int pass = 0;
            int fail = 0;
            int skip = 0;
            long total = 0;
            Stopwatch sw = Stopwatch.StartNew();
            foreach(PerftTest test in tests) {
                i++;
                if(test.expectedResult <= 200000) {
                    int result = Test(test.depth, test.board);
                    total += result;
                    if(result == test.expectedResult) {
                        Console.ForegroundColor = ConsoleColor.Green;
                        Console.WriteLine("Test " + i + " Passed");
                        Console.ResetColor();
                        pass++;
                    } else {
                        Console.ForegroundColor = ConsoleColor.Red;
                        Console.WriteLine("Test " + i + " Failed, Outputted " + result + " With FEN " + test.board.GetFenString() + " and Depth " + test.depth + ", Expected Result was " + test.expectedResult);
                        Console.ResetColor();
                        fail++;
                    }
                } else {
                    Console.WriteLine("Test too large to perform as part of a suite");
                    skip++;
                }
            }
            Console.WriteLine("Passed " + pass + ", Failed " + fail + ", Skipped " + skip);
            Console.WriteLine("Tests took " + sw.Elapsed);
            Console.WriteLine("Total nodes: " + total);
        }
        public static void PerformTest(PerftTest test) {
            int result = Test(test.depth, test.board);
            if(result == test.expectedResult) {
                Console.ForegroundColor = ConsoleColor.Green;
                Console.WriteLine("Test Passed");
                Console.ResetColor();
            } else {
                Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine("Test Failed, Outputted " + result + " With FEN " + test.board.GetFenString() + " and Depth " + test.depth + ", Expected Result was " + test.expectedResult);
                Console.ResetColor();
            }
        }
        public static int Test(int depth, Board board) { 
            Move[] moves = board.GetMoves();
            int count = 0;
            if(depth == 0) {
                return 1;
            }
            foreach(Move move in moves) {
                if(board.MakeMove(move)) {
                    count += Test(depth - 1, board);
                    board.UndoMove(move);
                }
            }
            return count;
        }
    }
}