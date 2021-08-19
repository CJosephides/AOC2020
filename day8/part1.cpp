#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>


class Instruction {
    public:
        long acc_delta;
        long idx_delta;

        Instruction(long acc_delta, long idx_delta)
            :acc_delta {acc_delta}, idx_delta {idx_delta} {};

        friend std::ostream& operator<<(std::ostream&, const Instruction&);
};


std::ostream& operator<<(std::ostream& os, const Instruction& instruction) {
    os <<  "(accumulator delta: " << instruction.acc_delta << ", index delta: " << instruction.idx_delta << ")";
    os << '\n';
    return os;
}


Instruction instruction_from_line(const std::string& line) {
    
    std::string sub = line.substr(0, 3);

    if (sub == "nop") {
        return Instruction(0, 1);
    } else if (sub == "acc") {
        sub = line.substr(4, line.size() - 4);
        long acc_delta = std::stoi(sub);
        return Instruction(acc_delta, 1);
    } else {  // jmp
        sub = line.substr(4, line.size() - 4);
        long idx_delta = std::stoi(sub);
        return Instruction(0, idx_delta);
    }

}


void populate_instruction_vector(std::ifstream& filestream, std::vector<Instruction>& instructions) {
    std::string line;
    while (std::getline(filestream, line)) {
        instructions.push_back(instruction_from_line(line));
    }
}


int run_until_infinite_loop(std::vector<Instruction>& instructions) {
    long current_idx = 0;
    long current_acc = 0;
    std::unordered_set<int> visited_idx; 

    while (!visited_idx.count(current_idx)) {
        visited_idx.insert(current_idx);

        Instruction& instruction = instructions[current_idx];
        current_idx += instruction.idx_delta;
        current_acc += instruction.acc_delta;
    }

    return current_acc;
}


int main(int argc, char** argv) {
    std::ifstream filestream(argv[1]);
    if (!filestream.is_open()) {
        std::cout << "failed to open file " << argv[1] << " for reading.";
    }

    std::vector<Instruction> instructions;
    populate_instruction_vector(filestream, instructions);

    for (long i=0; i < instructions.size(); i++) {
        std::cout << "Instruction at index " << i << ": " << instructions[i];
    }

    std::cout << "\nExecuting instructions until infinite loop... ";
    std::cout << "accumulator value before loop = " << run_until_infinite_loop(instructions) << ".\n";

}
