#include <iostream>
#include <cstdint>

// this is the code that I used to generate my passed pawn masks.
uint64_t fileMask = 0x0101010101010101;

uint64_t getFileMask(int file) {
	return fileMask << file;
}

uint64_t getPassedPawnMask(int square, int colorToMove) {
	int squareRank = (square >> 3);
    int squareFile = (square & 7);
	uint64_t fileMask = getFileMask(squareFile);
    uint64_t rightMask = 0;
    uint64_t leftMask = 0;
    if(squareFile != 7) rightMask = getFileMask(squareFile + 1); 
    if(squareFile != 0) leftMask = getFileMask(squareFile - 1);
    uint64_t total = fileMask | rightMask | leftMask;
    uint64_t forwardMask = 0xffffffffffffffff << (8 * squareRank + 1);
    uint64_t backwardMask = 0xffffffffffffffff >> (8 * (8 - squareRank));
    if(squareRank != 0 && squareRank != 7) {
      if(colorToMove == 0) {
          return total & backwardMask;
      } else {
          return total & forwardMask;
      }
    } else {
    	return 0;
    }
}

int main() {
	for(int i = 0; i < 2; i++) {
    	std::cout << "{";
    	for(int j = 0; j < 64; j++) {
        	std::cout << std::to_string(getPassedPawnMask(j, i)) << ", ";
        }
        std::cout << "},\n";
    }
}
