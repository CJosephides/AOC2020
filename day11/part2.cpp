#include <iostream>
#include <fstream>
#include <string>
#include <vector>


enum Cell {
    floor,
    empty,
    filled,
};


bool assert_matrices_equal(std::vector<std::vector<Cell>> matrix, std::vector<std::vector<Cell>> other) {
    for (int i = 0; i < matrix.size(); i++) {
        for (int j = 0; j < matrix.front().size(); j++) {
            if (matrix[i][j] != other[i][j]) return false;
        }
    }
    return true;
}


class Grid {
    std::vector<std::vector<Cell>> matrix;

    public:
        int n_rows;
        int n_columns;

        void fill_grid_from_file(std::ifstream& filestream) {
            // Read file and fill in the grid cells.
            matrix.clear();
            std::string line;
            while (std::getline(filestream, line)) {
                // Start a new row.
                n_columns = 0;
                n_rows++;
                std::vector<Cell> current_line;
                for (auto char_itr = line.cbegin(); char_itr != line.cend(); char_itr++) {
                    // Read characters (columns).
                    n_columns++;
                    if (*char_itr == '.') current_line.push_back(Cell::floor);
                    else if (*char_itr == 'L') current_line.push_back(Cell::empty);
                    else current_line.push_back(Cell::filled);
                }
                matrix.push_back(current_line);
            }
        }

        friend std::ostream& operator<<(std::ostream&, Grid&);

        int count_visible_filled(int i, int j) {
            int n_visible_filled = 0;
            // up
            for (int ii = i - 1; ii >= 0; ii--) {
                if (matrix[ii][j] == Cell::filled) {
                    n_visible_filled++;
                    break;
                } 
                else if (matrix[ii][j] == Cell::empty) {
                    break;
                }
            }
            // up + right
            int delta = 1;
            while (true) {
                if (((i - delta) < 0) || ((j + delta) >= n_columns)) break;
                if (matrix[i - delta][j + delta] == Cell::filled) {
                    n_visible_filled++;
                    break;
                }
                else if (matrix[i - delta][j + delta] == Cell::empty) {
                    break;
                }
                delta++;
            }
            // right
            for (int jj = j + 1; jj < n_columns; jj++) {
                if (matrix[i][jj] == Cell::filled) {
                    n_visible_filled++;
                    break;
                }
                else if (matrix[i][jj] == Cell::empty) {
                    break;
                }
            }
            // down + right
            delta = 1;
            while (true) {
                if (((i + delta) >= n_rows) || ((j + delta) >= n_columns)) break;
                if (matrix[i + delta][j + delta] == Cell::filled) {
                    n_visible_filled++;
                    break;
                }
                else if (matrix[i + delta][j + delta] == Cell::empty) {
                    break;
                }
                delta++;
            }
            // down
            for (int ii = i + 1; ii < n_rows; ii++) {
                if (matrix[ii][j] == Cell::filled) {
                    n_visible_filled++;
                    break;
                }
                else if (matrix[ii][j] == Cell::empty) {
                    break;
                }
            }
            // down + left
            delta = 1;
            while (true) {
                if (((i + delta) >= n_rows) || ((j - delta) < 0)) break;
                if (matrix[i + delta][j - delta] == Cell::filled) {
                    n_visible_filled++;
                    break;
                }
                else if (matrix[i + delta][j - delta] == Cell::empty) {
                    break;
                }
                delta++;
            }
            // left
            for (int jj = j - 1; jj >= 0; jj--) {
                if (matrix[i][jj] == Cell::filled) {
                    n_visible_filled++;
                    break;
                }
                else if (matrix[i][jj] == Cell::empty) {
                    break;
                }
            }
            // left + up
            delta = 1;
            while (true) {
                if (((i - delta) < 0) || ((j - delta) < 0)) break;
                if (matrix[i - delta][j - delta] == Cell::filled) {
                    n_visible_filled++;
                    break;
                }
                else if (matrix[i - delta][j - delta] == Cell::empty) {
                    break;
                }
                delta++;
            }
            return n_visible_filled; 
        }

        bool simulate(void) {
            std::vector<std::vector<Cell>> new_matrix = matrix;  // copy the matrix

            for (int i = 0; i < n_rows; i++) {
                for (int j = 0; j < n_columns; j++) {
                    if (matrix[i][j] == Cell::floor) continue;  // floor... floor never changes...

                    // Either an empty or a filled seat.
                    int n_visible_filled = count_visible_filled(i, j);
                    if (matrix[i][j] == Cell::empty) {
                        // An empty seat.
                        if (n_visible_filled == 0) new_matrix[i][j] = Cell::filled;
                    }
                    else {
                        // A filled seat.
                        if (n_visible_filled >= 5) new_matrix[i][j] = Cell::empty;
                    }
                }
            }
            // Check matrix equality here.
            bool no_change = assert_matrices_equal(matrix, new_matrix);
            matrix = new_matrix;
            return no_change;
        }

        int count_filled(void) {
            int n_filled = 0;
            for (auto row : matrix) {
                for (auto cell : row) {
                    if (cell == Cell::filled) n_filled++;
                }
            }
            return n_filled;
        }
};


std::ostream& operator<<(std::ostream& os, Grid& grid) {
    for (int i = 0; i < grid.n_rows; i++) {
        for (int j = 0; j < grid.n_columns; j++) {
            if (grid.matrix[i][j] == Cell::floor) os << '.';
            else if (grid.matrix[i][j] == Cell::empty) os << 'L';
            else os << '#';
        }
        os << '\n';
    }
    return os;
}


int main(int argc, char** argv) {

    std::ifstream filestream(argv[1]);
    if (!filestream.is_open()) {
        std::cout << "could not open " << argv[1] << " for reading.\n";
    }

    Grid grid;  // uses default constructor
    grid.fill_grid_from_file(filestream);
    std::cout << "Iteration 0.\n";
    std::cout << grid; 

    bool no_change = false;
    int iteration_number = 0;
    do {
        iteration_number++;
        no_change = grid.simulate();
        std::cout << "\nIteration " << iteration_number << ".\n";
        std::cout << grid; 
    } while (!no_change);

    std::cout << "There are " << grid.count_filled() << " occupied seats, after " << iteration_number - 1 << " iterations.\n";
}
