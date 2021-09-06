#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <unordered_map>
#include <functional>


const int NN = 80;  // number of neighbors for each cube


struct Coordinate {
    int x;
    int y;
    int z;
    int w;

    public:
        bool operator==(const Coordinate& other) const {
            return ((x == other.x) && (y == other.y) && (z == other.z) && (w == other.w));
        }
        
};


struct CoordinateHash {
    std::size_t operator()(const Coordinate& coordinate) const {
        return ((std::hash<int>()(coordinate.x) ^ (std::hash<int>()(coordinate.y) << 1)) >> 1) ^ ((std::hash<int>()(coordinate.z) << 1) >> 2) ^
                ((std::hash<int>()(coordinate.w) << 1) >> 1);
    }
};


class Cube {

    public:
        Coordinate coordinate;
        bool state;

    public:
        Cube(int x, int y, int z, int w, bool state = true) {
            coordinate.x = x;
            coordinate.y = y;
            coordinate.z = z;
            coordinate.w = w;
            this->state = state;
        }
        Cube() {
            coordinate.x = 0;
            coordinate.y = 0;
            coordinate.z = 0;
            coordinate.w = 0;
            state = false;
        }

    public:
        std::array<Coordinate, NN> get_neighbors(void) {
            std::array<Coordinate, NN> neighbors;
            int c = 0;
            for (int i = -1; i < 2; i++) {
                for (int j = -1; j < 2; j++) {
                    for (int k = -1; k < 2; k++) {
                        for (int q = -1; q < 2; q++) {
                            if ((i == 0) && (j == 0) && (k == 0) && (q == 0)) continue;
                            neighbors[c] = Coordinate {coordinate.x + i, coordinate.y + j, coordinate.z + k, coordinate.w + q};
                            c++;
                        }
                    }
                }
            }
            return neighbors;
        }

        bool operator==(const Cube& other) const {
            return ((coordinate.x == other.coordinate.x) && (coordinate.y == other.coordinate.y) && (coordinate.z == other.coordinate.z) && (coordinate.w == other.coordinate.w));
        }

    public:
};



class Grid {
    private:
        std::unordered_map<Coordinate, Cube, CoordinateHash> cubes;
    public:
        Grid(std::ifstream& ifs) {
            int x = 0;
            int y = 0;
            Coordinate coordinate;
            std::string line;
            while (std::getline(ifs, line)) {
                x = 0;
                for (auto c: line) {
                    if (c == '#') {
                        coordinate = {x, y, 0, 0}; 
                        cubes[coordinate] = Cube(x, y, 0, 0, true);  // assume z = w = 0
                    }
                    x++;
                }
                y++;
            }
        }

    friend std::ostream& operator<<(std::ostream&, Grid&);

    void update_rule_1(std::vector<Coordinate>& to_activate, std::vector<Coordinate>& to_inactivate) {
        int n_neighbors;
        for (auto cube_iter = cubes.begin(); cube_iter != cubes.end(); cube_iter++) {
            n_neighbors = 0;
            for (Coordinate neighbor_coordinate : cube_iter->second.get_neighbors()) {
                if (cubes.find(neighbor_coordinate) != cubes.end()) n_neighbors++;
            }
            if ((n_neighbors != 2) && (n_neighbors != 3)) {
                to_inactivate.push_back(cube_iter->first);
            }
        }
    }

    void update_rule_2(std::vector<Coordinate>& to_activate, std::vector<Coordinate>& to_inactivate) {
        std::unordered_map<Coordinate, int, CoordinateHash> potentials;
        for (auto [coordinate, cube] : cubes) {
            for (auto neighbor_coordinate : cube.get_neighbors()) {
                if (cubes.find(neighbor_coordinate) == cubes.end()) {
                    if (potentials.find(neighbor_coordinate) != potentials.end()) 
                        potentials[neighbor_coordinate]++;
                    else
                        potentials[neighbor_coordinate] = 1;
                }
            }
        }

        for (auto [coordinate, potential] : potentials) {
            if (potential == 3) to_activate.push_back(coordinate);
        }
    }

    void update(void) {
        std::vector<Coordinate> to_activate;
        std::vector<Coordinate> to_inactivate;
        update_rule_1(to_activate, to_inactivate);
        update_rule_2(to_activate, to_inactivate);

        // inactivate
        for (auto coordinate: to_inactivate) {
            cubes.erase(coordinate);
        }
        // activate
        for (auto coordinate: to_activate) {
            cubes[coordinate] = Cube(coordinate.x, coordinate.y, coordinate.z, coordinate.w, true);
        }
    }

    int count_cubes(void) {
        return cubes.size();
    }
};


std::ostream& operator<<(std::ostream& os, Grid& grid) {
    for (auto [coordinate, cube] : grid.cubes) {
        os << "active cube @ (" << cube.coordinate.x << ", " << cube.coordinate.y << ", " << cube.coordinate.z << ", " << cube.coordinate.w << ")" << std::endl;
    }
    return os;
}


int main(int argc, char** argv) {

    std::ifstream ifs(argv[1]);
    Grid grid(ifs);
    std::cout << "initial grid" << std::endl;
    std::cout << grid << std::endl;

    for (int i = 0; i < 6; i++) {
        grid.update();
    }

    std::cout << grid << std::endl;
    std::cout << "there are " << grid.count_cubes() << " active cubes." << std::endl;
}
