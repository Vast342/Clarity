using System.Numerics;
namespace Chess {
    public class RayGenerator {
        public static ulong GetRookAttacks(int startSquare, ulong occupiedBitboard) {
            // North
            ulong north = Mask.slideyPieceRays[0, startSquare];
            ulong potentialBlockers = occupiedBitboard & north;
            int firstBlocker = BitOperations.TrailingZeroCount(potentialBlockers | (1UL << 63));
            north ^= Mask.slideyPieceRays[0, firstBlocker];
            // South
            ulong south = Mask.slideyPieceRays[1, startSquare];
            potentialBlockers = occupiedBitboard & south;
            firstBlocker = BitOperations.LeadingZeroCount(potentialBlockers | 1);
            south ^= Mask.slideyPieceRays[1, 63-firstBlocker];
            // East
            ulong east = Mask.slideyPieceRays[2, startSquare];
            potentialBlockers = occupiedBitboard & east;
            firstBlocker = BitOperations.TrailingZeroCount(potentialBlockers | (1UL << 63));
            east ^= Mask.slideyPieceRays[2, firstBlocker];
            // West
            ulong west = Mask.slideyPieceRays[3, startSquare];
            potentialBlockers = occupiedBitboard & west;
            firstBlocker = BitOperations.LeadingZeroCount(potentialBlockers | 1);
            west ^= Mask.slideyPieceRays[3, 63-firstBlocker];

            return north | south | east | west;
        }
        public static ulong GetBishopAttacks(int startSquare, ulong occupiedBitboard) {
            // North-West
            ulong northWest = Mask.slideyPieceRays[4, startSquare];
            ulong potentialBlockers = occupiedBitboard & northWest;
            int firstBlocker = BitOperations.TrailingZeroCount(potentialBlockers | (1UL << 63));
            northWest ^= Mask.slideyPieceRays[4, firstBlocker];
            // South-East
            ulong southEast = Mask.slideyPieceRays[5, startSquare];
            potentialBlockers = occupiedBitboard & southEast;
            firstBlocker = BitOperations.LeadingZeroCount(potentialBlockers | 1);
            southEast ^= Mask.slideyPieceRays[5, 63-firstBlocker];
            // North-East
            ulong northEast = Mask.slideyPieceRays[6, startSquare];
            potentialBlockers = occupiedBitboard & northEast;
            firstBlocker = BitOperations.TrailingZeroCount(potentialBlockers | (1UL << 63));
            northEast ^= Mask.slideyPieceRays[6, firstBlocker];
            // South-West
            ulong southWest = Mask.slideyPieceRays[7, startSquare];
            potentialBlockers = occupiedBitboard & southWest;
            firstBlocker = BitOperations.LeadingZeroCount(potentialBlockers | 1);
            southWest ^= Mask.slideyPieceRays[7, 63-firstBlocker];

            return northWest | southEast | northEast | southWest;
        }
        public static void GetAllRookAttacks() {

        }
        public static void GetAllBishopAttacks() {
            Console.BufferHeight = short.MaxValue-1;
            for(int i = 0; i < 32; i++) {
                ulong attackRay = 0;
                for(int j = 4; j < 8; j++) {
                    attackRay |= Mask.slideyPieceRays[j, i];
                }
                ulong[] possibleBlockers = CreateAllBlockerBitboards(attackRay);
                Console.WriteLine("{");
                foreach(ulong blockers in possibleBlockers) {
                    Console.WriteLine(GetBishopAttacks(i, blockers) + ",");
                }
                Console.WriteLine("},");
            }
            Console.WriteLine("Got all blocker bitboards for all squares");
        }
        public static ulong[] CreateAllBlockerBitboards(ulong movementMask) {
            // creates a list of the squares in the movementMask
            List<int> moveableSquareIndices = new();
            for(int index = 0; index < 64; index++) {
                if(((movementMask >> index) & 1) == 1) {
                    moveableSquareIndices.Add(index);
                }
            }
            // calculates the total amount of possible blocker configuration
            int totalPatterns = 1 << moveableSquareIndices.Count;
            ulong[] blockerBitboards = new ulong[totalPatterns];

            // generates each possible configuration
            for(int patternNumber = 0; patternNumber < totalPatterns; patternNumber++) {
                for(int bitNumber = 0; bitNumber < moveableSquareIndices.Count; bitNumber++) {
                    int currentBit = (patternNumber >> bitNumber) & 1;
                    blockerBitboards[patternNumber] |= (ulong)currentBit << moveableSquareIndices[bitNumber];
                } 
            }  

            return blockerBitboards;
        }
    }
}