#include <string>
#include <iostream>
#include <fstream>


enum Operator{ addition, multiplication };


class Parser {

    int position;
    std::string string;

    public:

    Parser(std::string string) {
        position = 0;
        this->string = string;
    }

    public:

    long long get_operand() {
        int start = 0;
        int stop = 0;
        long long operand = 0;
        
        // ignore leading whitespace
        for (char c : string.substr(position, string.npos)) {
            if (c != ' ') break;
            start++;
        }

        // check if we have a nested expression and evaluate it
        if (string[position + start] == '(') {
            //std::cout << "opening parens at position " << position + start << std::endl;
            position += start + 1; 
            return evaluate_expression();
        }
    
        // read until whitespace
        for (char c: string.substr(position + start, string.npos)) {
            if (c == ' ') {
                break;
            }
            else if (c == ')') {
                //std::cout << "closing parens at position " << position + start + stop << std::endl;
                break;
            }
            stop++;
        }
        
        operand = std::stoi(string.substr(position + start, stop));
        position += start + stop;

        return operand;
    
    };

    Operator get_operator() {
        int start = 0;
        int stop = 0;
        
        // ignore leading whitespace
        for (char c : string.substr(position, string.npos)) {
            if (c != ' ') break;
            start++;
        }
    
        // read until operator
        for (char c: string.substr(position + start, string.npos)) {
            if (c == '+') {
                (position) += start + stop + 1;
                return Operator::addition;
            }
            else if (c == '*') {
                (position) += start + stop + 1;
                return Operator::multiplication;
            }
            stop++;
        }
    };

    long long evaluate_expression() {
        long long left = 0;
        long long right = 0;
        Operator op;

        //std::cout << "evaluating expression" << std::endl;

        left = get_operand();
        //std::cout << "left = " << left << ", position = " << position << std::endl;

        while (position < string.size() - 1) {
            // check if we are done
            if (string[position] == ')') {
                position++;
                break;
            }

            op = get_operator();
            //std::cout << "operator = " << op << ", position = " << position << std::endl;
            right = get_operand();
            //std::cout << "right = " << right << ", position = " << position << std::endl;

            switch (op) {
                case (Operator::addition):
                    left += right;
                    break;
                case (Operator::multiplication):
                    left *= right;
                    break;
            }
            //std::cout << "new left = " << left << std::endl;
        }
        //std::cout << "finished evaluating (" << left << ")" << std::endl;
        return left;
    }

    long long parse(void) {
        return evaluate_expression();
    }

};


int main(int argc, char** argv) {
    // tests
    //std::string string = "      9   +   7";
    //std::string string = "      9   +   ( 7 * 2)";
    //std::string string = "1 + 2 * 3 + 4 * 5 + 6";
    //std::string string = "1 + (2 * 3) + (4 * (5 + 6))";
    //std::string string = "5 + (8 * 3 + 9 + 3 * 4 * 3)";
    //std::string string = "5 * 9 * (7 * 3 * 3 + 9 * 3 + (8 + 6 * 4))";
    //std::string string = "((2 + 4 * 9) * (6 + 9 * 8 + 6) + 6) + 2 + 4 * 2";
    //std::string string = "(4 * (6 * 5 + 8 + 7 * 4) * (5 + 4 + 3 * 7 * 8 * 4) + 6 * (3 + 6) * 8) + 3 + (5 * (6 + 6 + 8) * 4) + 2 * ((8 * 9 * 8) + (9 * 8 * 9 * 9 + 8 + 7) + 7)";
    //Parser parser(string);
    //std::cout  << parser.parse() << std::endl;
    
    std::string line;
    long long result;
    long long sum = 0;
    std::ifstream fs(argv[1]);

    while (std::getline(fs, line)) {
        std::cout <<  line << " = ";
        Parser parser(line);
        result = parser.parse();
        std::cout << result << std::endl;
        sum += result;
    }
    
    std::cout << "Sum = " << sum << std::endl;
    
}
