#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <bitset>
#include <unordered_map>
#include <vector>



std::vector<unsigned long long> mask_value(std::string mask, unsigned long long value) {
    std::vector<unsigned long long> masked_values;
    std::bitset<36> bit_value {value};
    std::bitset<36> tmp_bit_value {value};
    char c;
    for (int i = 0; i < 36; i++) {
        c = mask[i];
        switch (c) {
            case 'X':
                break;
            case '0':
                break;
            case '1':
                bit_value[35-i] = true;
                break;
        }
    }

    std::vector<unsigned long long> new_masked_values;

    for (int i = 0; i < 36; i++) {
        c = mask[i];
        switch (c) {
            case 'X':
                if (masked_values.empty()) {
                    // nothing yet, use bit_value as base
                    tmp_bit_value = bit_value;
                    masked_values.push_back(tmp_bit_value.to_ullong());
                    masked_values.push_back(tmp_bit_value.flip(35-i).to_ullong());
                } else {
                    // we have addresses to modify
                    for (auto mv_ptr = masked_values.begin(); mv_ptr != masked_values.end(); mv_ptr++) {
                        tmp_bit_value = *mv_ptr;
                        tmp_bit_value.flip(35-i);
                        new_masked_values.push_back(bit_value.to_ullong());
                    }
                    for (auto nmv : new_masked_values) {
                        masked_values.push_back(nmv);
                    }
                }
                break;
            case '0':
                break;
            case '1':
                break;
        }
    }
    return masked_values;
}


int main(int argc, char** argv) {

    std::ifstream fs(argv[1]);
    if (!fs.is_open()) {
        std::cout << "couldn't open " << argv[1] << " for reading." << std::endl;
    }

    std::unordered_map<int, std::vector<unsigned long long>> addresses;
    unsigned long long value;
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
            for (auto mv_ptr = addresses[memory_address].begin(); mv_ptr != addresses[memory_address].end(); mv_ptr++) {
                std::cout << "New write at address " << memory_address << " = " << *mv_ptr << std::endl;
            }
        }
    }


    long long leftover_sum = 0;
    for (auto [address, values] : addresses) {
        for (auto val_ptr = values.begin(); val_ptr != values.end(); val_ptr++) {
            leftover_sum += *val_ptr;
        }
    }
    std::cout << "Leftover sum = " << leftover_sum << std::endl;

}

