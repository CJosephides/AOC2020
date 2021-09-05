#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <bitset>
#include <unordered_map>



unsigned long long mask_value(std::string mask, unsigned long long value) {
    std::bitset<36> bit_value {value};
    char c;
    for (int i = 0; i < 36; i++) {
        c = mask[i];
        switch (c) {
            case 'X':
                break;
            case '0':
                bit_value[35-i] = false;
                break;
            case '1':
                bit_value[35-i] = true;
                break;
        }
    }
    return bit_value.to_ullong();
}


int main(int argc, char** argv) {

    std::ifstream fs(argv[1]);
    if (!fs.is_open()) {
        std::cout << "couldn't open " << argv[1] << " for reading." << std::endl;
    }

    std::unordered_map<int, long long> addresses;
    long long value;
    int memory_address;
    std::string current_mask;
    std::string line;
    while (std::getline(fs, line)) {
        if (line.substr(0, 4) == "mask") {
            // New mask.
            current_mask = line.substr(7, std::string::npos);
            std::cout << "New mask = " << current_mask << std::endl;
        } else {
            // We have a write.
            memory_address = std::stoi(line.substr(4, line.find_first_of(']') - 4));
            value = std::stoi(line.substr(line.find_first_of('=') + 2, std::string::npos));
            addresses[memory_address] = mask_value(current_mask, value);
            std::cout << "New write at address " << memory_address << " = " << addresses[memory_address] << std::endl;
        }
    }


    long long leftover_sum = 0;
    for (auto [address, value] : addresses) {
        leftover_sum += value;
    }
    std::cout << "Leftover sum = " << leftover_sum << std::endl;

}

