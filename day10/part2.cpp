#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unordered_map>


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
    int target = numbers.back() + 3;
    numbers.push_back(target);

    std::cout << "Sorted inputs:\n";
    for (auto n : numbers) {
        std::cout << n << ", ";
    }
    std::cout << '\n';

    // Start a map from number to number of paths to the target.
    std::unordered_map<int, long long> paths;
    for (auto n : numbers) {
        paths[n] = 0;
    }

    // Start from the greatest number before the target.
    long long n_paths;
    long long current_number;
    auto i = numbers.rbegin();
    i++;

    for (i; i != numbers.rend(); i++) {
        n_paths = 0;
        current_number = *i;

        for (int j = 1; j < 4; j++) {
            if ((current_number + j) > target) continue;
            if ((current_number + j) == target) n_paths++;
            if ((current_number + j) < target) n_paths += paths[current_number + j];
        }

        paths[current_number] = n_paths;
    }

    std::cout << paths[0] << "\n";

}
