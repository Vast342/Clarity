using Chess;
public class UCI {
    static void Main() {
        ChessEngine engine = new(0);
        while(true) {
            string? entry = Console.ReadLine();
            string? command = entry!.Split(' ')[0];
            if(entry == "uci") {
                engine.IdentifyUCI();
                Console.WriteLine("uciok");
            }
            if(command == "quit") {
                return;
            }
            if(command == "isready") {
                Console.WriteLine("readyok");
            }
            if(command == "ucinewgame") {
                engine.NewGame();
            }
            if(command == "position") {
                engine.LoadPosition(entry);
            }
            if(command == "test") {
                if(entry.Split(' ')[1] == "board-rep") {
                    Tests.BackendTests();
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
                        foreach(Move m in b.GetMoves()) {
                            if(b.MakeMove(m)) {
                                int results = Perft.Test(int.Parse(segments[1]) - 1, b);
                                b.UndoMove(m);
                                Console.WriteLine(m.ConvertToLongAlgebraic() + ": " + results);
                                total += results;
                            }
                        }
                    } else {
                        total = Perft.Test(int.Parse(segments[1]), b);
                    }
                    Console.WriteLine("");
                    Console.WriteLine("Total: " + total);
                } else {
                    int total = 0;
                    Board b = new(segments[2] + " " + segments[3] + " " + segments[4] + " " + segments[5] + " " + segments[6] + " " + segments[7]);  
                    if(segments.Length > 8) {
                        foreach(Move m in b.GetMoves()) {
                            if(b.MakeMove(m)) {
                                int results = Perft.Test(int.Parse(segments[1]) - 1, b);
                                b.UndoMove(m);
                                Console.WriteLine(m.ConvertToLongAlgebraic() + ": " + results);
                                total += results;
                            }
                        } 
                    } else {
                        total = Perft.Test(int.Parse(segments[1]), b);
                    }
                    Console.WriteLine("");
                    Console.WriteLine("Total: " + total);
                }
            }
            if(command == "perft-suite") {
                if(entry.Split(' ')[1] == "ethereal") {
                    Perft.PerformTestSuite(Suite.etherealSuite);
                }
            }
            if(command == "in-check") {
                string[] segments = entry.Split(" ");
                Board b = new(segments[1] + " " + segments[2] + " " + segments[3] + " " + segments[4] + " " + segments[5] + " " + segments[6]);
                Console.WriteLine(b.IsInCheck());
            }
            if(command == "go") {
                engine.Think(entry);
            }
            if(command == "get-fen") {
                engine.GetFen();
            }
            if(command == "make-move") {
                engine.MakeMove(entry);
            }
            if(command == "generate-bishop-attacks") {
                RayGenerator.GetAllBishopAttacks();
            }
            if(command == "generate-rook-attacks") {
                RayGenerator.GetAllRookAttacks();
            }
            if(command == "find-magics") {
                
            }
        }
    }
}
