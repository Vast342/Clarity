public class UCI {
    static void Main() {
        TestBot.ComputeMasks();
        while(true) {
            string? entry = Console.ReadLine();
            string command = entry.Split(' ')[0];
            if(entry == "uci") {
                Console.WriteLine("id name I-don");
                Console.WriteLine("id author Vast");
                Console.WriteLine("uciok");
            }
            if(command == "quit") {
                return;
            }
            if(command == "isready") {
                TestBot.Initialize();
                Console.WriteLine("readyok");
            }
            if(command == "ucinewgame") {
                TestBot.NewGame();
            }
            if(command == "position") {
                TestBot.LoadPosition(entry);
            }
            if(command == "test") {
                if(entry.Split(' ')[1] == "board-rep") {
                    Tests.BackendTests();
                } else if(entry.Split(' ')[1] == "move-gen") {
                    Tests.MoveGenTests();
                } else if(entry.Split(' ')[1] == "move-masks") {
                    Tests.MoveMaskTests();
                } else if(entry.Split(' ')[1] == "outliers") {
                    Tests.OutlierTests();
                }
            }
            if(command == "go") {
                TestBot.Think();
            }
            if(command == "get-fen") {
                TestBot.GetFen();
            }
        }
    }
}
