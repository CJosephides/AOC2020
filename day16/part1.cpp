#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <vector>


class Rule {
    private:
        std::pair<int, int> lower_range;
        std::pair<int, int> upper_range;

    public:
        Rule(const std::pair<int, int>& lower_range, const std::pair<int, int>& upper_range)
            : lower_range(lower_range), upper_range(upper_range) {};

        bool is_valid(int number) {
            if ((lower_range.first <= number) && (number <= lower_range.second)) return true;
            if ((upper_range.first <= number) && (number <= upper_range.second)) return true;
            return false;
        }

        Rule(const std::string& line) {
            std::string token;
            int lower_range_first;
            int lower_range_second;
            int upper_range_first;
            int upper_range_second;

            token = line.substr(line.find(':') + 2, line.find('-', line.find(':') + 2) - 1);
            lower_range_first = std::stoi(token);

            token = line.substr(line.find('-') + 1, line.find(' ', line.find(':') + 1) - 1);
            lower_range_second = std::stoi(token);

            token = line.substr(line.find(" or ") + 4, line.find('-', line.find(" or ") + 4) - 1);
            upper_range_first = std::stoi(token);

            token = line.substr(line.find('-', line.find(" or ")) + 1, std::string::npos);
            upper_range_second = std::stoi(token);

            lower_range = std::pair<int, int>(lower_range_first, lower_range_second);
            upper_range = std::pair<int, int>(upper_range_first, upper_range_second);
        }

    friend std::ostream& operator<<(std::ostream&, const Rule& rule);
};


std::ostream& operator<<(std::ostream& os, const Rule& rule) {
    os << "Rule: ";
    os << rule.lower_range.first << " -- " << rule.lower_range.second << " or "
       << rule.upper_range.first << " -- " << rule.upper_range.second << std::endl;
    return os;
}


int number_error_rate(int number, const std::vector<Rule>& rules) {
    bool no_validating_rule;
    no_validating_rule = true;
    for (auto rule : rules) {
        if (rule.is_valid(number)) {
            no_validating_rule = false;
            break;
        }
    }
    if (no_validating_rule) return number;
    return 0;
}


int line_error_rate(const std::string& line, const std::vector<Rule>& rules) {

    int number;
    int error_rate = 0;
    size_t token_start = 0;
    size_t token_end = line.find(',', token_start);

    while (token_end != std::string::npos) {
        // read numbers
        number = std::stoi(line.substr(token_start, token_end - token_start));
        error_rate += number_error_rate(number, rules);
        token_start = token_end + 1;
        token_end = line.find(',', token_start);
    }
    // read last number
    number = std::stoi(line.substr(token_start, std::string::npos));
    error_rate += number_error_rate(number, rules);
    return error_rate;
}


int main(int argc, char** argv) {
    int number_of_rules = std::atoi(argv[2]);
    std::ifstream fs(argv[1]);
    std::string line;
    
    std::vector<Rule> rules;
    int current_line = 0;

    // read rules
    for (; current_line < number_of_rules; current_line++) {
        std::getline(fs, line);
        rules.push_back(Rule(line));
        std::cout << rules.back();
    }

    // skip own ticket for now
    for (; current_line < number_of_rules + 5; current_line++) {
        std::getline(fs, line);
    }

    // read nearby tickets
    int error_rate = 0;
    while (std::getline(fs, line)) {
        error_rate += line_error_rate(line, rules); 
    }
    std::cout << "error rate = " << error_rate << std::endl;
    
}
