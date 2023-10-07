#include "tuner.h"

/*
    LIST OF THINGS TO DO / FIX
    double check / fix sigmoid function DONE

    Question, can I count the number of lines in the file? that would make it much faster 
    I think
    
    Initialize it into a std::vector<Board> so that I don't need to reparse fen strings.

    I didn't mess up the values, oops. I have more stuff to figure out.


*/

double sigmoidScale = 1.5;

// SiGmOiD
double sigmoid(int value) {
    return (1/(1+pow(10, (-sigmoidScale * value) / 400)));
}

double calculateAverageError() {
    std::ifstream dataset{"F:/filtered-lichess-big3-resolved.txt"};

    double total = 0;
    if(dataset.is_open()) {
        std::string line;
        int i = 0;
        while(getline(dataset, line)) {
            i++;
            //std::cout << "testing position " << std::to_string(i);
            std::vector<std::string> segments = split(line, ' ');
            Board testingBoard(segments[0] + " " + segments[1] + " " + segments[2] + " " + segments[3] + " " + segments[4] + " " + segments[5]);
            double datasetResult = std::stod(segments[6]);
            //int qSearchResult = deterministicQSearch(testingBoard, -10000000, 10000000);
            double sigmoidified = sigmoid(testingBoard.getEvaluation());
            //std::cout << ", error of " << std::to_string(pow(datasetResult-sigmoidified, 2)) << '\n';
            total += pow(datasetResult - sigmoidified, 2);
        }
        dataset.close();
        total /= i;
    } else {
        std::cout << "file didn't open????\n";
    }
    return total;
}
/*
void filterData() {
    std::ifstream dataset{"F:/ChessEngineAgain/src/lichess-big3-resolved.txt"};
    std::ofstream filtered{"F:/filtered-lichess-big3-resolved.txt"};

    int i = 0;
    int j = 0;
    if(dataset.is_open()) {
        std::string line;
        while(getline(dataset, line)) {
            i++;
            std::vector<std::string> segments = split(line, ' ');
            Board testingBoard(segments[0] + " " + segments[1] + " " + segments[2] + " " + segments[3] + " " + segments[4] + " " + segments[5]);
            int qSearchResult = deterministicQSearch(testingBoard, -10000000, 10000000);
            if(qSearchResult == testingBoard.getEvaluation()) {
                j++;
                std::cout << "added position " << std::to_string(i) << " to filtered data\n";
                filtered << line << '\n';
            }
        }
        dataset.close();
    } else {
        std::cout << "file didn't open????\n";
    }
    std::cout << "Data filtered, total lines: " << std::to_string(i) << ", Lines added: " << std::to_string(j) << '\n';
}*/

void tuneK() {
    std::cout << "Beginning K Tuning:\n ";
    double increment = 0.1;
    double bestValueOfK = 0;
    double bestError = 10000000;
    for(double i = 0.1; i < 4; i += increment) {
        std::cout << "Testing scale of " << std::to_string(i); 
        sigmoidScale = i;
        double error = calculateAverageError();
        std::cout << " average error is " << std::to_string(error) << '\n';
        if(error <= bestError) {
            bestValueOfK = i;
            bestError = error;
            std::cout << "New best K found: " << std::to_string(bestValueOfK) << " with an error of " << std::to_string(bestError) << '\n';
        }
    }
    std::cout << "Round 1 done, best value found is " << std::to_string(bestValueOfK) << " with an error of " << std::to_string(bestError) << '\n';
    double firstFoundValueOfK = bestValueOfK;
    increment = 0.01;
    for(double i = firstFoundValueOfK-0.1; i < firstFoundValueOfK+0.1; i += increment) {
        std::cout << "Testing scale of " << std::to_string(i); 
        sigmoidScale = i;
        double error = calculateAverageError();
        std::cout << " average error is " << std::to_string(error) << '\n';
        if(error <= bestError) {
            bestValueOfK = i;
            bestError = error;
            std::cout << "New best K found: " << std::to_string(bestValueOfK) << " with an error of " << std::to_string(bestError) << '\n';
        }
    }
    std::cout << "Round 2 done, best value found is " << std::to_string(bestValueOfK) << " with an error of " << std::to_string(bestError) << '\n';
    firstFoundValueOfK = bestValueOfK;
    increment = 0.001;
    for(double i = firstFoundValueOfK-0.01; i < firstFoundValueOfK+0.01; i += increment) {
        std::cout << "Testing scale of " << std::to_string(i); 
        sigmoidScale = i;
        double error = calculateAverageError();
        std::cout << " average error is " << std::to_string(error) << '\n';
        if(error <= bestError) {
            bestValueOfK = i;
            bestError = error;
            std::cout << "New best K found: " << std::to_string(bestValueOfK) << " with an error of " << std::to_string(bestError) << '\n';
        }
    }
    std::cout << "Round 3 done, best value found is " << std::to_string(bestValueOfK) << " with an error of " << std::to_string(bestError) << '\n';
    firstFoundValueOfK = bestValueOfK;
    increment = 0.0001;
    for(double i = firstFoundValueOfK-0.001; i < firstFoundValueOfK+0.001; i += increment) {
        std::cout << "Testing scale of " << std::to_string(i); 
        sigmoidScale = i;
        double error = calculateAverageError();
        std::cout << " average error is " << std::to_string(error) << '\n';
        if(error <= bestError) {
            bestValueOfK = i;
            bestError = error;
            std::cout << "New best K found: " << std::to_string(bestValueOfK) << " with an error of " << std::to_string(bestError) << '\n';
        }
    }
    std::cout << "Round 4 done, best value found is " << std::to_string(bestValueOfK) << " with an error of " << std::to_string(bestError) << '\n';
    firstFoundValueOfK = bestValueOfK;
    increment = 0.00001;
    for(double i = firstFoundValueOfK-0.0001; i < firstFoundValueOfK+0.0001; i += increment) {
        std::cout << "Testing scale of " << std::to_string(i); 
        sigmoidScale = i;
        double error = calculateAverageError();
        std::cout << " average error is " << std::to_string(error) << '\n';
        if(error <= bestError) {
            bestValueOfK = i;
            bestError = error;
            std::cout << "New best K found: " << std::to_string(bestValueOfK) << " with an error of " << std::to_string(bestError) << '\n';
        }
    }
    std::cout << "Computation done\n";
    std::cout << "Best value of K found: " << std::to_string(bestValueOfK) << '\n';
    std::cout << "Best error found: " << std::to_string(bestError) << '\n';
}
