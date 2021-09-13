#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <cctype>


class Rule {
    public:
        int number;
        std::vector<std::vector<int>> children_set;
        char emits;

    public:
        Rule(int number, std::vector<std::vector<int>> children_set, char emits)
            : number(number), children_set(children_set), emits(emits) {};
        Rule(int number, char emits)
            : number(number), emits(emits), children_set({}) {};
        Rule(int number, std::vector<std::vector<int>> children_set)
            : number(number), children_set(children_set), emits(0) {};
        Rule() {};

    friend std::ostream& operator<<(std::ostream&, Rule);

};


std::ostream& operator<<(std::ostream& os, Rule rule) {
    os << rule.number << ": ";
    if (rule.children_set.empty())
        os << rule.emits;
    else {
        for (auto children : rule.children_set) {
            for (auto child : children) {
                os << child << " ";
            }
            os << "| ";
        }
    }
    return os;
}


class Ruleset {

    public:
        std::unordered_map<int, Rule> rules;

    public:
        Ruleset(std::ifstream& fs) {
            // Create Ruleset from a file of rules.
            std::string line;
            while (std::getline(fs, line)) {
                if (isdigit(line[0])) {
                    // We have a rule.
                    Rule rule = rule_from_line(line); 
                    rules[rule.number] = rule;
                } else 
                    break;  // reached end of rules (assuming all rules come before any messages
            }
        }

        Ruleset() {}

    private:
        Rule rule_from_line(std::string line) {
            int number;
            char emits;
            size_t index;

            // Get the rule number.
            index = line.find(':');
            number = std::stoi(line.substr(0, index));

            // Start reading children or emitted character.
            index += 1;
            while (line[index] == ' ') index++;

            // Check if this rule simply emits a char.
            if (line[index] == '"') {
                emits = line[index+1];  // assume single emitted char
                return Rule(number, emits);
            }


            // Otherwise, rule has children.
            std::vector<std::vector<int>> children_set;
            std::vector<int> children;
            int child_number;
            size_t n_chars;
            char c;

            while (index < line.size()) {
                c = line[index];
                if (c == ' ') {
                    // Nothing to do here.
                    index++;
                } else if (isdigit(c)) {
                    // Read until next whitespace.
                    n_chars = 1;
                    while (index + n_chars < line.size()) {
                        if (line[index + n_chars] == ' ') break;
                        n_chars++;
                    }
                    child_number = std::stoi(line.substr(index, n_chars));

                    // Add this child to the current vector of children.
                    children.push_back(child_number);
                    // Increment index to one past the child number.
                    index += n_chars;
                    continue;
                } else if (c == '|') {
                    // Start a new vector of children and save the current one into the set of children.
                    children_set.push_back(children);
                    children.clear();
                    index++;
                }
            }
            // Push the final children to the set of children.
            children_set.push_back(children);

            // Done reading children.
            return Rule(number, children_set);
        }

    private:
        std::vector<size_t> check_rule(int rule_number, const std::string& message, std::vector<size_t> positions,
                                       int depth = 0) {
            // Checks if a rule, with number rule_number, is satisfied with the message at various index positions.
            // Returns a vector of new positions for future checks.
            Rule rule = rules[rule_number];

            // This is probably not the best solution, but it guarantees that long paths will get terminated at some
            // point.
            if (depth > message.size()) {
                return std::vector<size_t> {};
            }

            if (rule.children_set.empty()) {
                // Rule has no children, so check if it emits the correct character from each position.
                std::vector<size_t> new_positions;
                for (auto position : positions) {
                    if (rule.emits == message[position]) {
                        if ((position + 1) <=  message.size())
                            // Don't follow positions that have exceeded the length of the message.
                            new_positions.push_back(position + 1);
                    }
                }

                return new_positions;
            }

            // Otherwise the rule has at least one set of children. We want to check each sequence of children and
            // combine the valid positions from each to return to our caller.
            std::vector<size_t> combined_positions;
            for (auto children : rule.children_set) {
                // This is the same as the above single-sequence case.
                std::vector<size_t> tmp_positions = positions;
                for (auto child_number : children) {
                    tmp_positions = check_rule(child_number, message, tmp_positions, depth + 1);
                }
                // Copy any valid positions.
                for (auto position : tmp_positions) combined_positions.push_back(position);
            }
            return combined_positions;
        }

    public:
        bool is_valid_message(const std::string& message) {
            // Checks if a message is valid given the ruleset, starting with rule 0 (at position 0).
            std::vector<size_t> initial_positions = {0};
            std::vector<size_t> final_positions;
            final_positions = check_rule(0, message, initial_positions);
            // Check if any final position has reached the end (one past) of the message.
            for (auto fp : final_positions) {
                if (fp == message.size()) return true;
            }
            return false;
        }
};


int main(int argc, char** argv) {

    std::ifstream fs(argv[1]);
    std::string line;

    Ruleset ruleset(fs);;
    std::cout << "Ruleset" << std::endl;
    std::cout << "=======" << std::endl;
    for (auto [number, rule] : ruleset.rules) {
        std::cout << rule << std::endl;
    }
    std::cout << "=======" << std::endl;
    
    int n_matches = 0;

   while (std::getline(fs, line)) {
       if (line.empty()) continue;
       std::cout << line << " : ";
       if (ruleset.is_valid_message(line)) {
           std::cout << "is valid!";
           n_matches++;
       }
       else
           std::cout << "not valid.";
       std::cout << std::endl;
   }
   std::cout << n_matches << " valid messages." << std::endl;
}
