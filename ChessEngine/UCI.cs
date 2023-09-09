using Chess;
public static class UCI {
    static ChessEngine engine = new(0);
    static Board board = new("8/8/8/8/8/8/8/8 w - - 0 1");
    static public int numMoves = 0;
    static void Main() {
        MagicGeneration.GenerateMasks();
        MagicGeneration.InitializeSavedMagics();
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
                LoadPosition(entry);
            }
            if(command == "test") {
                if(entry.Split(' ')[1] == "board-rep") {
                    Tests.BackendTests();
                } else if(entry.Split(' ')[1] == "outliers") {
                    Tests.OutlierTests();
                }
            }
            if(command == "perft") {
                PerformPerft(entry);
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
                engine.Think(entry, board);
            }
            if(command == "get-fen") {
                GetFen();
            }
            if(command == "make-move") {
                MakeMove(entry);
            }
        }
    }
    // I seperated this out into it's own class to avoid the compiler warning because of my usage of stackalloc here.
    public static void PerformPerft(string entry) {
        string[] segments = entry.Split(' ');
        if(segments[2] == "startpos") {
            int total = 0;
            Board b = new("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
            if(segments.Length > 3) {
                Span<Move> moves = stackalloc Move[256];
                b.GetMoves(ref moves);
                foreach(Move m in moves) {
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
                Span<Move> moves = stackalloc Move[256];
                b.GetMoves(ref moves);
                foreach(Move m in moves) {
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
    /// <summary>
    /// Gets the fen string of the position currently being viewed by the bot
    /// </summary>
    public static void GetFen() {
        Console.WriteLine(board.GetFenString());
    }
    /// <summary>
    /// detects a move from the command, and does it on the board.
    /// </summary>
    /// <param name="entry">The command being sent</param>
    public static void MakeMove(string entry) {
        board.MakeMove(new Move(entry.Split(' ')[1], board));
        Console.WriteLine(board.GetFenString());
    }
    /// <summary>
    /// Loads the position from a position command
    /// </summary>
    /// <param name="position">The position command</param>
    public static void LoadPosition(string position) {
        // THIS ENTIRE THING NEEDS TO BE REWRITTEN, FENS DON'T WORK, AND IT NEEDS TO LOAD IN ALL THE MOVES, NOT JUST ONE
        string[] segments = position.Split(' ');
        if(segments[1] == "startpos") {
            // set the board to be a board that is the in the starting position
            board = new("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
            if(segments.Length > 2) {
                if(segments[2] == "moves") {
                    // loops through each move each time
                    for(int i = 3; i < segments.Length; i++) {
                        board.MakeMove(new Move(segments[i], board));
                        numMoves++;
                    }
                }
            }
        } else if(segments[1] == "fen") {
            // gets each segment of the fen string
            board = new(segments[2] + " " + segments[3] + " " + segments[4] + " " + segments[5] + " " + segments[6] + " " + segments[7]);
            // loops through the same exact way
            if(segments.Length > 8) { 
                if(segments[8] == "moves") {
                    for(int i = 9; i < segments.Length; i++) {
                        board.MakeMove(new Move(segments[i], board));
                        numMoves++;
                    }
                }
            }
        }
    }
}