#include <fstream>
#include <iostream>
#include <list>
#include <stdlib.h>


template<class T>
class NumberStream {
    std::list<T> numbers;
    int max_length;

    public:
        NumberStream (const int max_length)
            :max_length {max_length} {}

        bool validate_new(const long num) {
            for (auto i = numbers.begin(); i != numbers.end(); i++) {
                auto j = i;
                for (j++; j != numbers.end(); j++ ) {
                    if ((*i + *j == num) && (*i != *j)) return true;
                }
            }
            return false;
        }

        void insert_new(const long num) {
            numbers.push_back(num);
            if (numbers.size() > max_length) {
                numbers.pop_front();
            }
        }
};


long read_until_first_invalid(std::ifstream& filestream, int max_length) {
    long next_num;
    NumberStream<long> number_stream(max_length);

    // Read preamble.
    std::cout << "Reading preamble...\n";
    while (max_length) {
        filestream >> next_num;
        std::cout << "read " << next_num << '\n';
        number_stream.insert_new(next_num);
        max_length--;
    }

    // Read remaining.
    std::cout << "Reading remaining...\n";
    while (filestream >> next_num) {
        std::cout << "read " << next_num << '\n';
        if (!number_stream.validate_new(next_num)) {
            return next_num;
        }
        number_stream.insert_new(next_num);
    }

    return -1;
}


int main(int argc, char**argv) {
    
    std::ifstream filestream(argv[1]);  // input file
    int max_length = strtol(argv[2], nullptr, 10);  // max number stream & preamble length

    long first_invalid = read_until_first_invalid(filestream, max_length);
    std::cout << "First invalid number is " << first_invalid << '\n';
}
