#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <list>


enum class InstructionType { nop, acc, jmp};

std::ostream& operator<<(std::ostream& os, const InstructionType& instructiontype) {
    switch (instructiontype) {
        case InstructionType::nop:
            os << "nop";
            break;
        case InstructionType::acc:
            os << "acc";
            break;
        case InstructionType::jmp:
            os << "jmp";
            break;
    }
    return os;
}

class Instruction {
    public:
        InstructionType type;
        long acc_delta;
        long idx_delta;

        Instruction(const std::string& type_name, long acc_delta, long idx_delta)
            :acc_delta {acc_delta}, idx_delta {idx_delta} {
                if (type_name == "nop") type = InstructionType::nop;
                else if (type_name == "acc") type = InstructionType::acc;
                else type = InstructionType::jmp;
            };

        friend std::ostream& operator<<(std::ostream&, const Instruction&);
};


std::ostream& operator<<(std::ostream& os, const Instruction& instruction) {
    os << "{" << instruction.type << "} (accumulator delta: " << instruction.acc_delta << ", index delta: " << instruction.idx_delta << ")";
    os << '\n';
    return os;
}


Instruction instruction_from_line(const std::string& line) {
    
    std::string sub = line.substr(0, 3);

    if (sub == "nop") {
        return Instruction("nop", 0, 1);
    } else if (sub == "acc") {
        sub = line.substr(4, line.size() - 4);
        long acc_delta = std::stoi(sub);
        return Instruction("acc", acc_delta, 1);
    } else {  // jmp
        sub = line.substr(4, line.size() - 4);
        long idx_delta = std::stoi(sub);
        return Instruction("jmp", 0, idx_delta);
    }

}


void populate_instruction_vector(std::ifstream& filestream, std::vector<Instruction>& instructions) {
    std::string line;
    while (std::getline(filestream, line)) {
        instructions.push_back(instruction_from_line(line));
    }
}


bool run_until_infinite_loop_or_finished(std::vector<Instruction>& instructions, std::list<long>& visited_record,
                                         long& current_acc) {
    long current_idx = 0;
    current_acc = 0;  // reset to start
    std::unordered_set<long> visited_idx; 

    while (!visited_idx.count(current_idx)) {
        visited_idx.insert(current_idx);
        visited_record.push_back(current_idx);

        Instruction& instruction = instructions[current_idx];
        current_acc += instruction.acc_delta;
        current_idx += instruction.idx_delta;
        if (current_idx >= instructions.size()) return true;  // program terminated successfully
    }

    return false;  // program entered infinite loop
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

    // First run to record path.
    bool termination_status = false;
    long current_acc = 0;
    std::list<long> visited_record;

    std::cout << "\nExecuting instructions until infinite loop...";
    termination_status = run_until_infinite_loop_or_finished(instructions, visited_record, current_acc);

    std::cout << "\nReverse index path before infinite loop: ";

    for (auto iter = visited_record.rbegin(); iter != visited_record.rend(); iter++) {
        std::cout << *iter << ", ";
    }
    std::cout << '\n';

    // Subsequent runs to try modifying instructions.
    std::cout << "\nUnwinding reverse path with instruction modification...\n";
    std::list<long> tmp;

    for (auto iter = visited_record.rbegin(); iter != visited_record.rend(); iter++) {
        Instruction instruction = instructions[*iter];  // get the instruction at the index
        if (instruction.type == InstructionType::jmp) {
            // Switch to nop.
            std::vector<Instruction> new_instructions = instructions;
            new_instructions[*iter] = Instruction("nop", 0, 1);

            // Run with this new vector of instructions.
            current_acc = 0;
            termination_status = run_until_infinite_loop_or_finished(new_instructions, tmp, current_acc);
            if (termination_status) {
                std::cout << "Program terminated successfully, by modifying index " << *iter << ", with accumulator value = " << current_acc << ".\n";
                return 0;
            }
            
        } else if (instruction.type == InstructionType::nop) {
            // Switch to jmp.
            std::vector<Instruction> new_instructions = instructions;
            new_instructions[*iter] = Instruction("jmp", 0, instruction.acc_delta);

            // Run with this new vector of instructions.
            current_acc = 0;
            termination_status = run_until_infinite_loop_or_finished(new_instructions, tmp, current_acc);
            if (termination_status) {
                std::cout << "Program terminated successfully, by modifying index " << *iter << ", with accumulator value = " << current_acc << ".\n";
                return 0;
            }
        }
        
    }

    //std::cout << "\nExecuting instructions until infinite loop... ";
    //std::cout << "accumulator value before loop = " << run_until_infinite_loop(instructions) << ".\n";

}
