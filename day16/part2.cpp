#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <array>
#include <unordered_map>


const int N_FIELDS = 20;


class Rule {
    private:
        std::pair<int, int> lower_range;
        std::pair<int, int> upper_range;

    public:
        std::string name;
        Rule(const std::string name, const std::pair<int, int>& lower_range, const std::pair<int, int>& upper_range)
            : name(name), lower_range(lower_range), upper_range(upper_range) {};

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

            name = line.substr(0, line.find(':'));

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
    os << "Rule ";
    os << rule.name << ": ";
    os << rule.lower_range.first << " -- " << rule.lower_range.second << " or "
       << rule.upper_range.first << " -- " << rule.upper_range.second << std::endl;
    return os;
}


bool is_ticket_invalid(const std::array<int, N_FIELDS>& ticket, const std::vector<Rule>& rules) {
    bool at_least_one_matching_rule;
    for (int i = 0; i < N_FIELDS; i++) {
        at_least_one_matching_rule = false;
        for (auto rule : rules) {
            if (rule.is_valid(ticket[i])) {
                at_least_one_matching_rule = true;
                break;
            }
        }
        if (!at_least_one_matching_rule) return true;  // is invalid
    }
    return false;  // is valid
}


std::array<int, N_FIELDS>* read_ticket_line(std::string& line, const std::vector<Rule>& rules) {

    std::array<int, N_FIELDS>* ticket = new std::array<int, N_FIELDS>;
    int number;
    size_t token_start = 0;
    size_t token_end = line.find(',', token_start);

    int i = 0;
    while (token_end != std::string::npos) {
        // read numbers
        number = std::stoi(line.substr(token_start, token_end - token_start));
        (*ticket)[i] = number;
        token_start = token_end + 1;
        token_end = line.find(',', token_start);
        i++;
    }
    // read last number
    number = std::stoi(line.substr(token_start, std::string::npos));
    (*ticket)[i] = number;

    // check if ticket is completely invalid
    if (is_ticket_invalid(*ticket, rules)) {
        return nullptr;
    }
    return ticket;
}


class NearbyTickets {
    public:
        std::vector<std::array<int, N_FIELDS>*> tickets;

    NearbyTickets(std::ifstream& ifs, std::vector<Rule>& rules) {
        std::string line;
        std::array<int, N_FIELDS>* next_ticket;
        while (std::getline(ifs, line)) {
            // read and discard completely invalid tickets
            next_ticket = read_ticket_line(line, rules);
            if (next_ticket != nullptr) 
                tickets.push_back(next_ticket);
        }
    }

    ~NearbyTickets() {
        for (int i = 0; i < N_FIELDS; i++) {
            delete tickets[i];
        }
    }

    std::array<std::unordered_map<std::string, bool>, N_FIELDS> make_options_table(const std::vector<Rule>& rules) {

        // create the options table and initialize everything to true
        std::array<std::unordered_map<std::string, bool>, N_FIELDS> options;
        for (int i = 0; i < N_FIELDS; i++) {
            for (auto rule: rules) {
                options[i][rule.name] = true;
            }
        }

        // iterate through the nearby tickets now and update the options table
        for (int i = 0; i < N_FIELDS; i++) {
            // for reach of the fields on the nearby tickets
            for (auto rule : rules) {
                // for each rule
                for (int j = 0; j < tickets.size(); j++) {
                    // if at least one ticket's field number violates the rule, set that option to false
                    if (!rule.is_valid((*tickets[j])[i])) {
                        options[i][rule.name] = false;
                        // DEBUG
                        //std::cout << "ticket " << j << " field " << i <<  " number " << (*tickets[j])[i] << " violates rule " << rule.name << std::endl;
                        break;
                    }
                }
            }
        }
        return options;
    }

};


std::string single_true(const std::unordered_map<std::string, bool>& option_line) {
    int n_true = 0;
    for (auto [name, value] : option_line) {
        if (value) n_true++;
    }
    if (n_true == 1) {
        for (auto [name, value] : option_line) {
            if (value) return name;
        }
    }
    return "";
}


void print_options_table(const std::array<std::unordered_map<std::string, bool>, N_FIELDS>& options) {
    for (int i = 0; i < N_FIELDS; i++) {
        std::cout << "[" << i << "] ";
        for (auto [rule, value] : options[i]) {
            std::cout << rule << ": " << value << ", ";
        }
        std::cout << std::endl;
    }
}


void update_options_table(std::array<std::unordered_map<std::string, bool>, N_FIELDS>& options) {

    int iterations = 0;
    bool repeat;
    bool some_change;
    std::string single_true_name;

    do {
        repeat = false;
        some_change = false;  // a bit hacky
        for (int i = 0; i < N_FIELDS; i++) {
            single_true_name = single_true(options[i]);
            if (!single_true_name.empty()) {
                repeat = true;
                // this line has a single true rule
                for (int j = 0; j < N_FIELDS; j++) {
                    if (i == j) continue;
                    if (options[j][single_true_name]) some_change = true;
                    options[j][single_true_name] = false;
                }
            }
        }
        std::cout << std::endl << "update iteration " << iterations << std::endl;
        print_options_table(options);
        std::cout << std::endl;
        iterations++;
        if (!some_change) repeat = false;
    } while (repeat);
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
    NearbyTickets nb_tickets(fs, rules);
    std::cout << "read in " << nb_tickets.tickets.size() << " nearby tickets" << std::endl;

    std::cout << std::endl << "initial options table" << std::endl;
    auto options = nb_tickets.make_options_table(rules);
    print_options_table(options);
    std::cout << std::endl;

    // update options table
    update_options_table(options);

    // now read the departures and multiply own ticket by hand: 362974212989
    
}
