#include <iostream>
#include <fstream>

int main(int argc, char** argv) {

    int x, y;
    int X[200];
    
    // read file into array
    std::ifstream input_file;
    input_file.open("input.txt");
    for (int i=0; i < 200; i++) {
        input_file >> X[i];
    }
    input_file.close();

    //for (int i=0; i < 6; i++) {
    //    std::cout << X[i] << std::endl;
    //}

    // nested loop over array
    for (int i = 0; i < 200; i++) {
        for (int j = 0; j < 200; j++) {
            for (int k = 0; k < 200; k++) {
                if (X[i] + X[j] + X[k] == 2020) {
                    std::cout << X[i] * X[j] * X[k] << std::endl;
                    exit(0);
                }
            }
        }
    }

    // return product
    return -1;
    
}
