#include <fstream>
#include <iostream>
#include <list>
#include <stdlib.h>


int MAX_LEN = 1000;  // NumberStream max length for second pass


template<class T>
class NumberStream {
    int max_length;

    public:
    int numbers_read;
    std::list<T> numbers;
        NumberStream (const int max_length)
            :max_length {max_length}, numbers_read {0} {}

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
            numbers_read++;
        }
};


long read_until_first_invalid(std::ifstream& filestream, int max_length, NumberStream<long>& number_stream) {

    long next_num;

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


long find_consecutive_sum(std::ifstream& filestream, int numbers_read, long invalid_number) {
    NumberStream<long> number_stream(MAX_LEN);

    // Read up to the invalid number.
    long next_num;
    while (numbers_read) {
        filestream >> next_num; 
        number_stream.insert_new(next_num);
        numbers_read--;
    }

    // Iterate backwards from last valid number.
    long sum, smallest, largest;
    for (auto i = number_stream.numbers.rbegin(); i != number_stream.numbers.rend(); i++) {
        std::cout << "Iterating backwards from " << *i << ".\n";
        smallest = largest = *i;
        sum = *i;
        auto j = i;  // a reverse iterator, starting from i
        while (sum < invalid_number) {
            j++;
            sum += *j;
            if (*j < smallest) smallest = *j;
            if (*j > largest) largest = *j;
        }
        if ((sum == invalid_number) && (i != j)) return smallest + largest;
    }

    return -1;
}


int main(int argc, char**argv) {
    
    std::ifstream filestream(argv[1]);  // input file
    int max_length = strtol(argv[2], nullptr, 10);  // max number stream & preamble length

    // Read until the first invalid number.
    NumberStream<long> number_stream(max_length);
    long first_invalid = read_until_first_invalid(filestream, max_length, number_stream);
    std::cout << "First invalid number is " << first_invalid << ".\n";

    // Rewind stream.
    filestream.seekg(0);
    long min_max_sum = find_consecutive_sum(filestream, number_stream.numbers_read, first_invalid);
    std::cout << "Min + max of consecutive sum = " << min_max_sum << ".\n";
}
