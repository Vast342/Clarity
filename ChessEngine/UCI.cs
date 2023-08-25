using Chess;
public class UCI {
    static void Main() {
        ChessBot bot = new();
        bot.Initialize("I-Don", "Vast");
        while(true) {
            string? entry = Console.ReadLine();
            string? command = entry!.Split(' ')[0];
            if(entry == "uci") {
                bot.IdentifyUCI();
                Console.WriteLine("uciok");
            }
            if(command == "quit") {
                return;
            }
            if(command == "isready") {
                Console.WriteLine("readyok");
            }
            if(command == "ucinewgame") {
                bot.NewGame();
            }
            if(command == "position") {
                bot.LoadPosition(entry);
            }
            if(command == "test") {
                if(entry.Split(' ')[1] == "board-rep") {
                    Tests.BackendTests();
                } else if(entry.Split(' ')[1] == "move-gen") {
                    Tests.MoveGenTests();
                } else if(entry.Split(' ')[1] == "outliers") {
                    Tests.OutlierTests();
                }
            }
            if(command == "perft") {
                string[] segments = entry.Split(' ');
                if(segments[2] == "startpos") {
                    int total = 0;
                    Board b = new("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
                    if(segments.Length > 3) {
                        foreach(Move m in b.GetLegalMoves()) {
                            if(b.MakeMove(m)) {
                                int results = Tests.Perft(int.Parse(segments[1]) - 1, b);
                                b.UndoMove(m);
                                Console.WriteLine(m.ConvertToLongAlgebraic() + ": " + results);
                                total += results;
                            }
                        }
                    } else {
                        total = Tests.Perft(int.Parse(segments[1]), b);
                    }
                    Console.WriteLine("");
                    Console.WriteLine("Total: " + total);
                } else {
                    int total = 0;
                    Board b = new(segments[2] + " " + segments[3] + " " + segments[4] + " " + segments[5] + " " + segments[6] + " " + segments[7]);  
                    if(segments.Length > 8) {
                        foreach(Move m in b.GetLegalMoves()) {
                            if(b.MakeMove(m)) {
                                int results = Tests.Perft(int.Parse(segments[1]) - 1, b);
                                b.UndoMove(m);
                                Console.WriteLine(m.ConvertToLongAlgebraic() + ": " + results);
                                total += results;
                            }
                        } 
                    } else {
                        total = Tests.Perft(int.Parse(segments[1]), b);
                    }
                    Console.WriteLine("");
                    Console.WriteLine("Total: " + total);
                }
            }
            if(command == "in-check") {
                string[] segments = entry.Split(" ");
                Board b = new(segments[1] + " " + segments[2] + " " + segments[3] + " " + segments[4] + " " + segments[5] + " " + segments[6]);
                Console.WriteLine(b.IsInCheck());
            }
            if(command == "go") {
                bot.Think();
            }
            if(command == "get-fen") {
                bot.GetFen();
            }
            if(command == "make-move") {
                bot.MakeMove(entry);
            }
        }
    }
}
