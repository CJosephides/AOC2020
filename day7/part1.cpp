#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <utility>
#include <regex>
#include <fstream>


std::regex PARENT_PATTERN {R"(^(\w* \w*) bags)"};
std::regex CHILD_PATTERN {R"((\d+) (\w* \w*) bag)"};


class Bag {
    public:
        std::string name;
        std::vector<Bag*> parents;

        Bag(const std::string& name)
            :name {name} {};

        friend std::ostream& operator<<(std::ostream&, const Bag&);
};


std::ostream& operator<<(std::ostream& os, const Bag& bag) {
    if (!bag.parents.size()) {
        os << "Bag " << bag.name << " has no parents.";
    } else {
        os << "Bag " << bag.name << " has parents: ";
        for (Bag* parent: bag.parents) {
            os << parent->name << ", ";
        }
    }
    os << '\n';
    return os;
}



void _visit_bag(const Bag* bag, int& n_ancestors, std::unordered_set<std::string>& visited) {
    // Mark bag as visited and increment ancestor count.
    visited.insert(bag->name);
    n_ancestors++;

    for (auto parent : bag->parents) {
        // Visit each parent, if we haven't already visited it.
        auto search = visited.find(parent->name);
        if (search == visited.end()) {
            // We haven't, so visit.
            _visit_bag(parent, n_ancestors, visited);
        } // Otherwise, we have, so don't visit again.
    }
}


int count_ancestors(const Bag* bag) { 
    int n_ancestors = 0;
    std::unordered_set<std::string> visited;

    _visit_bag(bag, n_ancestors, visited);
    return n_ancestors - 1;  // not counting starting bag
}


void add_relationship(const std::string& parent_name,
                      const std::string& child_name,
                      std::unordered_map<std::string, Bag*>& bags) {
    Bag* child;
    Bag* parent;

    child = bags[child_name];

    if (child == nullptr) {
        child = new Bag(child_name);
        bags[child_name] = child;
    }

    parent = bags[parent_name];

    if (parent == nullptr) {
        parent = new Bag(parent_name);
        bags[parent_name] = parent;
    }

    child->parents.push_back(parent);
}


void parse_line(const std::string& line, std::unordered_map<std::string, Bag*>& bags) {

    std::string parent_name;
    std::string child_name;
    
    // Parent matching.
    auto bags_begin = std::sregex_iterator(line.begin(), line.end(), PARENT_PATTERN);
    auto bags_end = std::sregex_iterator();

    if (std::distance(bags_begin, bags_end) > 0) {
        for (std::sregex_iterator b = bags_begin; b != bags_end; b++) {
            parent_name = (*b)[1];
        }
    }

    // Children matching.
    for (std::sregex_iterator p(line.begin(), line.end(), CHILD_PATTERN); p != std::sregex_iterator{}; p++) {
        // std::cout << (*p)[1] << '\n'; // number of child bags
        child_name = (*p)[2];

        // Record this parent-child relationship.
        add_relationship(parent_name, child_name, bags);
    }

}


int main(int argc, char** argv) {

    std::unordered_map<std::string, Bag*> bags;

    std::ifstream filestream(argv[1]);

    if (!filestream.is_open()) {
        std::cout << "failed to open " << argv[1] << '\n';
    }

    std::string line;
    while (std::getline(filestream, line)) {
        parse_line(line, bags);
    }

    std::cout << "---------------------------------------------------------------------\n";
    for (const auto& bag_entry : bags) {
        std::cout << *(bag_entry.second);
    }
    std::cout << "---------------------------------------------------------------------\n";

    std::cout << "shiny gold has " << count_ancestors(bags["shiny gold"]) << " ancestors.\n";

}

