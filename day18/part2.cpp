#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <assert.h>
#include "part2.h"


enum Category { addition, multiplication, operand, none };


class Element {
    public:
        Category category;
        long long value;

    public:
        Element() :category(Category::none), value(0) {};
        Element(Category category, long long value) :category(category), value(value) {};


    friend std::ostream& operator<<(std::ostream&, Element);
};


std::ostream& operator<<(std::ostream& os, Element element) {
    switch (element.category) {
        case Category::addition:
            os << "+";
            break;
        case Category::multiplication:
            os << "*";
            break;
        case Category::operand:
            os << element.value;
            break;
    }
    return os;
}


class Stack {
    public:
        std::vector<Element> stack;

    public:
        void push_back(Element element) {
            stack.push_back(element);
        }

    public:
        Element get_previous_operator(void) {
            for (auto iter = stack.crbegin(); iter != stack.crend(); iter++) {
                if ((iter->category == Category::addition) || (iter->category == Category::multiplication)) {
                    return *iter;
                }
            }
        }

        void collapse(void) {
            Element left_element;
            Element operat;
            Element right_element;
            long long left = 0;
            long long right = 0;

            if (stack.size() == 1) {
                return;
            }

            auto iter = stack.rbegin();
            left += iter->value;
            iter++;

            while (iter != stack.rend()) {
                // read operator
                operat = *iter;
                iter++;
                // read right operand
                right = iter->value;
                switch (operat.category) {
                    case Category::addition:
                        left = left + right;
                        break;
                    case Category::multiplication:
                        left = left * right;
                        break;
                }
                iter++; 
            }
            stack.clear();
            stack.push_back(Element(Category::operand, left));
        };

        Element result(void) {
            assert(stack.size() == 1);
            return stack.front();
        }

    friend std::ostream& operator<<(std::ostream&, Stack);
};


std::ostream& operator<<(std::ostream& os, Stack stack) {
    for (auto iter = stack.stack.rbegin(); iter != stack.stack.rend(); iter++) {
        os << "(" << *iter << ") ";
    }
    return os;
}


size_t get_next_element(const std::string& string, size_t position, Element& element) {
    // Get the next element in string starting from position.
    // Returns the number of characters read; assigns to element.

    size_t read = 0;
    char c;

    // TODO stop at closing parens

    // ignore leading whitespace
    while (c = string[position + read]) {
        if (c == ' ') {
            read++;
        } else break;
    }

    // check if we have an operator
    if (c == '+') {
        element.category = Category::addition;
        element.value = 0;
        read++;
        return read;
    } else if (c == '*') {
        element.category = Category::multiplication;
        element.value = 0;
        read++;
        return read;
    }

    // check if we have an opening parens
    if (c ==  '(') {
        // step inside and parse recursively
        read++;
        size_t extra_read = parse(string, element, position + read);
        read += extra_read;
        return read;
    }

    // otherwise we have an operand
    size_t operand_start = read;
    while (c = string[position + read]) {
        if ((c == ' ') || (c == ')')) break;
        read++; 
    }

    long long value = std::stoi(string.substr(position + operand_start, read - operand_start));
    element.category = Category::operand;
    element.value = value;

    return read;
}

size_t parse(const std::string& string, Element& final_element, size_t position = 0) {
    Stack stack;

    Element element;
    size_t read;
    size_t original_position = position;

    //std::cout << "Parsing from position " << position << std::endl;

    while (position < string.size() - 1) {

        // check if we have reached a closing parens
        if (string[position] == ')') {
            //std::cout << "Reached closing parens. Collapse." << std::endl;
            stack.collapse();
            //std::cout << "Stack: " << stack << std::endl;
            element = stack.result();

            final_element.category = element.category;
            final_element.value = element.value;

            position++;
            //std::cout << "Finished parsing at position " << position << ". Result = " << final_element.value << std::endl;
            return position - original_position;
        }

        //std::cout << "@ position " << position << ". ";

        read = get_next_element(string, position, element);

        if (element.category == Category::multiplication) {
            if (stack.get_previous_operator().category == Category::addition) {
                //std::cout << "COLLAPSE DA STACK!!1!" << std::endl;
                stack.collapse();
            }
        }

        stack.push_back(element);  // copy

        //std::cout << "Next element: " << element << std::endl;

        //std::cout << "Stack: " << stack << std::endl;

        position += read;
    }

    //std::cout << "Reached end of string. Collapsy." << std::endl;
    stack.collapse();
    //std::cout << "Stack: " << stack << std::endl;
    element = stack.result();

    final_element.category = element.category;
    final_element.value = element.value;

    //std::cout << "Finished parsing at position " << position << ". Result = " << final_element.value << std::endl;
    return position - original_position;
};


int main(int argc, char** argv) {
    // Test cases.
    //Element result;
    //size_t position;
    //std::string string = "24 + 44 * 33";
    //std::string string = "2 + 3 + 4 * 5 * 6 + 7 + 8";
    //std::string string = "(1 + 2 * 3 + 4 * 5 + 6)";
    //std::string string = "2 * 3 + (4 * 5)";
    //std::string string = "5 + (8 * 3 + 9 + 3 * 4 * 3)";
    //std::string string = "5 * 9 * (7 * 3 * 3 + 9 * 3 + (8 + 6 * 4))";
    //std::string string = "((2 + 4 * 9) * (6 + 9 * 8 + 6) + 6) + 2 + 4 * 2";
    //position = parse(string, result);

    //std::cout << "Result = " <<  result.value << std::endl;
    //std::cout << "Position = " << position << std::endl;
    
    Element result;
    std::string line;
    std::ifstream fs(argv[1]);
    long long sum = 0;

    while (std::getline(fs, line)) {
        std::cout << line << " = ";
        parse(line, result);
        sum += result.value;
        std::cout << result.value << std::endl;
    }

    std::cout << "Sum = " << sum << std::endl;
}
