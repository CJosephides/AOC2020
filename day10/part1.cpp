#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>


int main(int argc, char** argv) {

    std::ifstream filestream(argv[1]);
    
    std::vector<int> numbers;
    int num;

    // Insert implied start.
    numbers.push_back(0);

    // Read all numbers.
    while (filestream >> num) {
        numbers.push_back(num);
    }

    // Sort them.
    std::sort(numbers.begin(), numbers.end());

    // Insert implied finish.
    numbers.push_back(numbers.back() + 3);

    for (auto n : numbers) {
        std::cout << n << ", ";
    }
    std::cout << '\n';

    int delta_ones = 0;
    int delta_threes = 0;

    for (int i = 0; i < numbers.size() - 1; i++) {
        if (numbers[i+1] - numbers[i] == 1) delta_ones++;
        else if (numbers[i+1] - numbers[i] == 3) delta_threes++;
        else return 1;
    }

    std::cout << "delta_ones * delta_threes = " << delta_ones * delta_threes << ".\n";
    
}
