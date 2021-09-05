#include <fstream>
#include <iostream>
#include <utility>
#include <unordered_map>


class NumberTracker {
    private:
        std::pair<long, long> turns;

    public:
        NumberTracker(long turn) {
            turns.first = 0;
            turns.second = turn;
        }
        
        NumberTracker() {
            turns.first = 0;
            turns.second = 0;
        }

        bool twice(void) {
            return (turns.first ? true : false);
        }

        void new_occurence(long turn) {
            turns.first = turns.second;
            turns.second = turn;
        }

        long difference(void) {
            return turns.second - turns.first;
        }
};


long next_number(std::unordered_map<long, NumberTracker>& numbers, long previous) {
    if (numbers.find(previous) == numbers.end()) {
        // this is a new number
        return 0;
    } else {
        // we have seen this number at least once before
        if (numbers[previous].twice()) {
            // at least twice
            return numbers[previous].difference();
        }
        // only once
        return 0;
    }
}

void handle_number(std::unordered_map<long, NumberTracker>& numbers, long number, long turn) {
    if (numbers.find(number) == numbers.end()) {
        // this is a new number
        numbers[number] = NumberTracker(turn);
    } else {
        // we have seen this number before
        numbers[number].new_occurence(turn);
    }
}


int main(int argc, char** argv) {
    long starters[] = {0, 3, 6};  // assume unique
    long turn = 0;
    long prev;
    long next;
    long n = sizeof(starters) / sizeof(long);

    std::unordered_map<long, NumberTracker> numbers;

    // Read in the starters first.
    for (long i = 0; i < n; i++) {
        turn++;
        handle_number(numbers, starters[i], turn);
        prev = starters[i];
        std::cout << "[" << turn << "] say " << prev << std::endl;
    }

    while (turn < std::atoi(argv[1])) {
        turn++;
        next = next_number(numbers, prev);
        handle_number(numbers, next, turn);
        prev = next;
        std::cout << "[" << turn << "] say " << prev << std::endl;
    }

}
