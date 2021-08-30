#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <regex>


int main(int argc, char** argv) {
    std::ifstream filestream(argv[1]);

    std::string line;

    // Get target number.
    std::getline(filestream, line);
    int target = std::stoi(line);

    // Get candidate numbers.
    std::getline(filestream, line);

    std::unordered_map<int, int> numbers;

    std::regex numbers_regex {"[^,|x]+"};
    auto numbers_begin = std::sregex_iterator(line.begin(), line.end(), numbers_regex);
    auto numbers_end = std::sregex_iterator();

    for (std::sregex_iterator i = numbers_begin; i != numbers_end; i++) {
        std::smatch match = *i;
        std::string match_str = match.str();
        numbers.insert({std::stoi(match_str), target});
    }
    
    // Iterate over the numbers and find distances.
    for (auto iter = numbers.begin(); iter != numbers.end(); ++iter) {
        int value = 0;
        while (value < target) {
            value += iter->first;
        }
        numbers[iter->first] = value - target;
    }

    // Print passing distance.
   for (auto [n, d] : numbers) {
       std::cout << "number " << n << " passing distance = " << d << '\n';
   }
}
