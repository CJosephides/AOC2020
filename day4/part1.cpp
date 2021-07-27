#include <iostream>
#include <fstream>
#include <string>
#include <memory>


void reset_flags(bool& byr, bool& iyr, bool& eyr, bool& hgt, bool& hcl, bool& ecl, bool& pid, bool& cid){
    byr = false;
    iyr = false;
    eyr = false;
    hgt = false;
    hcl = false;
    ecl = false;
    pid = false;
    cid = false;
}


bool check_flags(bool& byr, bool& iyr, bool& eyr, bool& hgt, bool& hcl, bool& ecl, bool& pid, bool& cid){
    if (byr && iyr && eyr && hgt && hcl && ecl && pid) return true;
    return false;
}


void print_flags(bool& byr, bool& iyr, bool& eyr, bool& hgt, bool& hcl, bool& ecl, bool& pid, bool& cid){
    std::cout << "byr: " << byr << " iyr: " << iyr << " eyr: " << eyr << " hgt: " << hgt << " hcl: " << hcl << " ecl: " << ecl << " pid: " << pid << " cid " << cid;
    if (check_flags(byr, iyr, eyr, hgt, hcl, ecl, pid, cid)) {
        std::cout << " ===";
    }
    std::cout << std::endl;
}


int main(int argc, char** argv) {

    std::string filename = argv[1];
    std::ifstream ifile(filename);
    std::string line;

    // flags
    int valid_passports = 0;
    bool byr = false;
    bool iyr = false;
    bool eyr = false;
    bool hgt = false;
    bool hcl = false;
    bool ecl = false;
    bool pid = false;
    bool cid = false;

    // string parsing
    std::string substr;
    size_t start;
    size_t end;

    //while (ifile >> line) {
    while (std::getline(ifile, line)) {

        // Passport end; check if we had a valid passport.
        if (line.length() == 0) {
            print_flags(byr, iyr, eyr, hgt, hcl, ecl, pid, cid);
            if (check_flags(byr, iyr, eyr, hgt, hcl, ecl, pid, cid)) valid_passports++;
            reset_flags(byr, iyr, eyr, hgt, hcl, ecl, pid, cid); 
            std::cout << std::endl;
            continue;
        }

        std::cout << line << std::endl;

        // Keep reading passport.
        start = 0u;
        end = line.find(':', start);
        while (end != std::string::npos) {
            substr = line.substr(end - 3, 3);
            if (substr == "byr") {
                byr = true;
            } else if (substr == "iyr") {
                iyr = true;
            } else if (substr == "eyr") {
                eyr = true;
            } else if (substr == "hgt") {
                hgt = true;
            } else if (substr == "hcl") {
                hcl = true;
            } else if (substr == "ecl") {
                ecl = true;
            } else if (substr == "pid") {
                pid = true;
            } else if (substr == "cid") {
                cid = true;
            }
            start = end + 1;
            end = line.find(':', start);
        }
         
    }

    // Check the last passport.
    print_flags(byr, iyr, eyr, hgt, hcl, ecl, pid, cid);
    if (check_flags(byr, iyr, eyr, hgt, hcl, ecl, pid, cid)) valid_passports++;
    reset_flags(byr, iyr, eyr, hgt, hcl, ecl, pid, cid); 
    
    std::cout << "Found " << valid_passports << " valid passports." << std::endl;
}
