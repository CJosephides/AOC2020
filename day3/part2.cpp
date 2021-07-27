#include <iostream>
#include <fstream>
#include <memory>

// Angle of movement.
const int angle_x = 3;
const int angle_y = 1;


int map_index(int i, int j, int width) {
    return j + i * width;
}


int descent(bool* map, int angle_y, int angle_x, int n_lines, int line_size) {

    // current positions
    int x = 0;  // 0 is left, line_size is right
    int y = 0;  // 0 is top, n_lines is bottom
    int n_trees = 0;

    do {
        if (map[map_index(y, x, line_size)] == 1) {
            n_trees++;
        }
        x += angle_x;
        x = x % line_size;
        y += angle_y;
    } while (y < n_lines);

    return n_trees;
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
    //bool* map = new bool[n_lines* line_size];
    std::unique_ptr<bool[]> map{new bool[n_lines * line_size]};

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

    // Start descents.
    long prod = 1;
    int descents[5][2] = {{1, 1}, {3, 1}, {5, 1}, {7, 1}, {1, 2}};
    for (int i = 0; i < 5; i++) {
        //prod *= descent(map, descents[i][1], descents[i][0], n_lines, line_size);
        prod *= descent(&map[0], descents[i][1], descents[i][0], n_lines, line_size);
    }

    std::cout << prod << std::endl;
}
