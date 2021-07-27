#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

int main(int argc, char** argv) {
    std::ifstream infile("input.txt");

    int i;
    int n_min, n_max;
    char sep, let, plet;
    int counts;
    int valid_passwords = 0;
    std::string line;
    std::string pass;

    while(std::getline(infile, line)) {
        //std::cout << line << std::endl;

        auto start = 0u;

        // min
        auto end = line.find('-'); 
        n_min = std::stoi(line.substr(start, end - start));
        //std::cout << n_min << std::endl;
        start = end + 1;

        // min
        end = line.find(' ');
        n_max = std::stoi(line.substr(start, end - start));
        //std::cout << n_max << std::endl;
        start = end + 1;

        // letter
        end = line.find(':');
        let = (line.substr(start, end - start))[0];
        //std::cout << let << std::endl;
        start = end + 1;

        // password
        end = line.find('\n');
        pass = line.substr(start + 1, end - start);
        //std::cout << pass << std::endl;

        // count occurences of letter
        counts = 0;
        i = 0;
        while (pass[i] != '\0') {
            if (pass[i] == let) counts++;
            i++;
        }

        if ((n_min <= counts) && (n_max >= counts)) {
            valid_passwords++;
        }
        
    }
    std::cout << "There are " << valid_passwords << " valid passwords." << std::endl;
}
