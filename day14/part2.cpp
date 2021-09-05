#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <bitset>
#include <unordered_map>
#include <vector>


std::vector<unsigned long long> mask_address(std::string mask, unsigned long long address) {
    std::bitset<36> bit_address {address};
    std::bitset<36> tmp_bit_address {address};
    std::vector<unsigned long long> masked_addresses;
    char c;

    // First pass to set any 1s
    for (int i = 0; i < 36; i++) {
        c = mask[i];
        switch (c) {
            case 'X':
                break;
            case '0':
                break;
            case '1':
                bit_address[35-i] = true;
                break;
        }
    }

    // Second pass to create floating addresses
    for (int i = 0; i < 36; i++) {
        c = mask[i];
        switch (c) {
            case 'X':
                if (masked_addresses.empty()) {
                    // use address
                    masked_addresses.push_back(bit_address.to_ullong());
                    masked_addresses.push_back(bit_address.flip(35-i).to_ullong());  // bit_address changed
                } else {
                    // fork existing addresses
                    std::vector<unsigned long long> new_masked_addresses;
                    for (auto a_ptr = masked_addresses.begin(); a_ptr != masked_addresses.end(); a_ptr++) {
                        tmp_bit_address = *a_ptr;  // hm, copy right?
                        new_masked_addresses.push_back(tmp_bit_address.flip(35-i).to_ullong());
                    }
                    // copy over
                    for (auto nma : new_masked_addresses) {
                        masked_addresses.push_back(nma);
                    }
                }
                break;
            case '0':
                break;
            case '1':
                break;
        }
    }
    return masked_addresses;
}


int main(int argc, char** argv) {

    std::ifstream fs(argv[1]);
    if (!fs.is_open()) {
        std::cout << "couldn't open " << argv[1] << " for reading." << std::endl;
    }

    std::unordered_map<unsigned long long, unsigned long long> addresses;
    unsigned long long value;
    unsigned long long memory_address;
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
            for (auto masked_address : mask_address(current_mask, memory_address)) {
                addresses[masked_address] = value; 
                std::cout << "New write at address " << masked_address << " = " << value << std::endl;
            }
        }
    }


    unsigned long long leftover_sum = 0;
    for (auto [address, value] : addresses) {
        leftover_sum += value;
    }
    std::cout << "Leftover sum = " << leftover_sum << std::endl;

}

