#include <iostream>
#include <fstream>

// Angle of movement.
const int angle_x = 3;
const int angle_y = 1;


int map_index(int i, int j, int width) {
    return j + i * width;
}


int main(int argc, char** argv) {

    std::string filename = argv[1];
    std::ifstream infile(filename);
    std::string line;

    unsigned int n_lines = 0;
    unsigned int line_size = 0;

    // First pass to figure out how many lines there are.
    while (infile >> line) {
        n_lines++;
        line_size = line.length();
    }

    std::cout << n_lines << " lines, with width " << line_size << std::endl;

    infile.close();
    infile.open(filename);

    // Second pass to read map to memory.
    int i;
    int j;
    bool* map = new bool[n_lines* line_size];

    i = 0;
    while (infile >> line) {
        std::cout << line << std::endl;
        for (j = 0; j < line_size; j++) {
            if (line[j] == '.') {
                map[map_index(i, j, line_size)] = 0;
            } else {
                map[map_index(i, j, line_size)] = 1;
            }
        }
        i++;
    }

    // Start descent.

    // current positions
    int x = 0;  // 0 is left, line_size is right
    int y = 0;  // 0 is top, n_lines is bottom
    int n_trees = 0;

    do {
        std::cout << "Current position is (" << y << ", " << x << ")." << std::endl;
        if (map[map_index(y, x, line_size)] == 1) {
            n_trees++;
            std::cout << "Hit tree! (" << n_trees << ")." << std::endl;
        }
        x += angle_x;
        x = x % line_size;
        y += angle_y;
    } while (y < n_lines);

    std::cout << "Hit " << n_trees << " trees on the way down." << std::endl;
}
