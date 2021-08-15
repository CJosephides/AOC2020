// Move to header file?
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>


class Group {
    private:
        int size;
        int responses[26];
    public:
        Group() {
            for (int i = 0; i < 26; i++) {
                responses[i] = 0;
            }
            size = 0;
        }
        int count_response_intersection() {
            int count = 0;
            for (int i = 0; i < 26; i++) {
                if (responses[i] == size) count++;
            }
            return count;
        }
        void parse_line(const std::string& line) {
            for (const char& c: line) {
                responses[c - 97]++;  // 97 is 'a', hopefully...
            }
            size++;
        }
        friend std::ostream& operator<<(std::ostream& os, const Group& group);
};


std::ostream& operator<<(std::ostream& os, const Group& group) {
    os << "size = " << group.size << ": ";
    for (int i = 0; i < 26; i++) {
        os << group.responses[i] << " ";
    }
    return os;
}


const std::vector<Group> query_groups_from_file(std::ifstream& filestream) {
    // Reads filestream and populates a vector of Groups with their responses.
    std::vector<Group> groups;
    std::string line;

    // Start a new group.
    Group current_group = Group();  // initialized with all 0 responses
    while (getline(filestream, line)) {
        std::cout << "Read: " << line << "\n";
        if (line.size() == 0) {
            // Encountered a blank line. Finalize group and start a new one.
            groups.push_back(current_group); // copies
            current_group = Group();
            continue;
        }
        // Otherwise we got a good line.
        current_group.parse_line(line);
    }

    // Finish the final group.
    groups.push_back(current_group); // copies

    return groups;
}


int main(int argc, char** argv) {
    
    // Open file.
    std::ifstream filestream(argv[1]);
    if (!filestream) {
        std::cout << "couldn't open " << argv[1] << " for reading.\n";
    }

    // Query groups in file.
    std::vector<Group> groups = query_groups_from_file(filestream);  // lots of copying

    // Get sum of counts of union of responses.
    int sum = 0;
    for (Group group : groups) {
        std::cout << "Group responses: " << group << "\n";
        sum += group.count_response_intersection();
    }
    std::cout << "Sum of counts of intersection responses = " << sum << ".\n";
}
