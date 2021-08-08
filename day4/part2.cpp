#include <iostream>
#include <fstream>
#include <unordered_set>


class Passport {
    private:
        std::unordered_set<std::string> contents;
    public:
        Passport() {}
        int insert(const std::string& key, const std::string& value) {
            if (key == "byr") {
                int yr = std::stoi(value);
                if (1920 <= yr && yr <= 2002) {
                    contents.insert(key);
                    return 0;
                } else 
                    return 1;
            }
            if (key == "iyr") {
                int yr = std::stoi(value);
                if (2010 <= yr && yr <= 2020) {
                    contents.insert(key);
                    return 0;
                } else 
                    return 1;
            }
            if (key == "eyr") {
                int yr = std::stoi(value);
                if (2020 <= yr && yr <= 2030) {
                    contents.insert(key);
                    return 0;
                } else 
                    return 1;
            }
            if (key == "hgt") {
                int height = stoi(value);
                if (value.find("cm") != std::string::npos) {
                    if (150 <= height && height <= 193) {
                        contents.insert(key);
                        return 0;
                    }
                } else if (value.find("in") != std::string::npos) {
                    if (59 <= height && height <= 76) {
                        contents.insert(key);
                        return 0;
                    }
                } else return 1;
            }
            if (key == "hcl") {
                if (value.size() != 7) return 1;
                if (value[0] != '#') return 1;
                std::string subvalue = value.substr(1, 6);
                auto pos = subvalue.find_first_not_of("0123456789abcdef");
                if (pos == std::string::npos) {
                    contents.insert(key);
                    return 0;
                } else return 1;
                
            }
            if (key == "ecl") {
                if ((value == "amb") || (value == "blu") || (value == "brn") || (value == "gry") || (value == "grn") || (value == "hzl") || (value == "oth")) {
                    contents.insert(key);
                    return 0;
                } else return 1;
            }
            if (key == "pid") {
                if (value.size() != 9) return 1;
                auto pos = value.find_first_not_of("0123456789");
                if (pos == std::string::npos) {
                    contents.insert(key);
                    return 0;
                } else return 1;
            }
            if (key == "cid") {
               contents.insert(key);
               return 0;
            }
            return 1;
        }
        bool is_valid() {
            auto search = contents.find("byr");
            if (search == contents.end()) return false;
            if (contents.find("iyr") == contents.end()) return false;
            if (contents.find("eyr") == contents.end()) return false;
            if (contents.find("hgt") == contents.end()) return false;
            if (contents.find("hcl") == contents.end()) return false;
            if (contents.find("ecl") == contents.end()) return false;
            if (contents.find("pid") == contents.end()) return false;
            return true;
        }
        void print() {
            for (auto iter = contents.begin(); iter != contents.end(); iter++) {
                std::cout << (*iter) << "; ";
            }
        }
};


int parse_line(std::ifstream& filestream, Passport& passport) {

    int num_kvps;
    std::string line;
    std::string key;
    std::string value;
    size_t pos = 0;
    size_t next_pos = 0;

    std::getline(filestream, line, '\n');
    if (line.size() == 0) return 0;

    std::cout << line << '\n';

    pos = 0;
    next_pos = 0;

    while (pos != std::string::npos) {
        next_pos = line.find(':', pos);
        key = line.substr(pos, next_pos - pos);
        pos = next_pos + 1;

        next_pos = line.find(' ', pos);
        value = line.substr(pos, next_pos - pos);

        passport.insert(key, value);
        num_kvps++;

        if (next_pos == std::string::npos) break;
        pos = next_pos + 1;

    }
    return num_kvps;
}


int main(int argc, char** argv) {
    std::ifstream filestream = std::ifstream(argv[1]);
    if (!filestream.is_open()){
        std::cout << "could not open " << argv[1] << " . Exiting.\n";
        return 1;
    }

    int num_kvps;
    int valid_passports = 0;
    std::cout << "Starting new passport...\n";
    Passport current_passport = Passport();

    while (!filestream.eof()){
        if (parse_line(filestream, current_passport) == 0) {
            std::cout << "Finished passport contents: ";
            current_passport.print();
            if (current_passport.is_valid()) {
                valid_passports++;
                std::cout << " passport is valid.\n";
            } else {
                std::cout << " passport is not valid.\n";
            }
            std::cout << "Starting new passport...\n";
            current_passport = Passport();
        }
    }
    std::cout << "There are " << valid_passports << " valid passports.\n";
}
