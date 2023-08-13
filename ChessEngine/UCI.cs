using System.Xml;

public class UCI {
    public static bool uci = false;
    static void Main() {
        while(true) {
            string entry = Console.ReadLine();
            if(entry == "uci") {
                uci = true;
                Console.WriteLine("uciok");
            }
            if(entry == "quit") {
                return;
            }
        }
    }
}
