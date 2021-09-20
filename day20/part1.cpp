#include <iostream>
#include <fstream>
#include <unordered_map>
#include <array>
#include <vector>
#include <string>
#include <stdexcept>
#include <cmath>


const int N = 10;
using barray = std::array<bool, N>;
using Face = std::vector<bool>;


std::ostream& operator<<(std::ostream& os, const Face& face) {
    for (bool element : face) {
        switch (element) {
            case false:
                os << '.';
                break;
            case true:
                os << '#';
                break;
        }
    }
    return os;
}

bool operator==(const Face& first, const Face& second) {
    if (first.size() != second.size())
        return false;
    for (int i = 0; i < first.size(); i++) {
        if (first[i] != second[i])
            return false;
    }
    return true;
}


class Tile {

    public:
    int id;
    std::array<barray, N> elements;

    public:
    Face north(void) {
        Face face;
        for (int i=0; i<N; i++) {
            face.push_back(elements[0][i]);
        }
        return face;
    };
    Face east(void) {
        Face face;
        for (int i=0; i<N; i++) {
            face.push_back(elements[i][N-1]);
        }
        return face;
    };
    Face south(void) {
        Face face;
        for (int i=0; i<N; i++) {
            face.push_back(elements[N-1][i]);
        }
        return face;
    };
    Face west(void) {
        Face face;
        for (int i=0; i<N; i++) {
            face.push_back(elements[i][0]);
        }
        return face;
    };

    void rotate_once(void) {
        std::array<barray, N> tmp_tile = this->elements;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                this->elements[j][N-1-i] = tmp_tile[i][j];
            }
        }
    }

    void rotate(int turns) {
        for (int i = 0; i < turns; i++) {
            rotate_once();
        }
    }

    void flip_ud(void) {
        barray tmp_row;
        for (int i = 0; i < N / 2; i++) {
            tmp_row = elements[i];
            elements[i] = elements[N - 1 - i];
            elements[N - 1- i] = tmp_row;
        }
    }

    friend std::ostream& operator<<(std::ostream&, const Tile&);

    public:
    Tile() {
        id = -1;
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                elements[i][j] = false;
    }

    public:
    static Tile from_file(std::ifstream& fs, std::string& line) {
        // Constructs a tile from an open filestream and the first line.
        Tile tile;
        tile.id = std::stoi(line.substr(5, line.npos - 1));
        std::getline(fs, line);
        char c;
        int i = 0;
        do {
            for (int j = 0; j < N; j++) {
                c = line[j];
                if (c == '.')
                    tile.elements[i][j] = false;
                else
                    tile.elements[i][j] = true;
            }
            std::getline(fs, line);
            i++;
        } while (!line.empty());
        return tile;
    }

    bool operator==(const Tile& other) {
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                if ((this->elements[i][j]) != other.elements[i][j]) return false;
        return true;
    }
};

std::ostream& operator<<(std::ostream& os, const Tile& tile) {
    for (int i=0; i<N; i++) {
        for (int j=0; j<N; j++) {
            switch (tile.elements[i][j]) {
                case false:
                    os << '.';
                    break;
                case true:
                    os << '#';
                    break;
            }
        }
        os << std::endl;
    }
    return os;
};


class Cluster {
    public:
    std::vector<std::vector<Tile>> tiles;

    Face north(void) {
        Face face;
        Tile tile;
        Face face_tile;
        for (int i = 0; i < tiles[0].size(); i++) {
            tile = tiles[0][i];
            face_tile = tile.north();
            for (auto element : face_tile)
                face.push_back(element);
        }
        return face;
    }

    Face east(void) {
        Face face;
        Tile tile;
        Face face_tile;
        for (int i = 0; i < tiles.size(); i++) {
            tile = tiles[i][tiles[0].size()-1];
            face_tile = tile.east();
            for (auto element : face_tile)
                face.push_back(element);
        }
        return face;
    }

    Face south(void) {
        Face face;
        Tile tile;
        Face face_tile;
        for (int i = 0; i < tiles[tiles.size()-1].size(); i++) {
            tile = tiles[tiles.size()-1][i];
            face_tile = tile.south();
            for (auto element : face_tile)
                face.push_back(element);
        }
        return face;
    }

    Face west(void) {
        Face face;
        Tile tile;
        Face face_tile;
        for (int i = 0; i < tiles.size(); i++) {
            tile = tiles[i][0];
            face_tile = tile.west();
            for (auto element : face_tile)
                face.push_back(element);
        }
        return face;
    }

    public:
    Cluster(Tile& first, Tile& second) {
        // assumes west -> east
        std::vector<Tile> row;
        row.push_back(first);
        Face first_east = first.east();
        Face second_west = second.west();
        if (first_east != second_west) {
            throw std::runtime_error("invalid tile border merge");
        }
        row.push_back(second);
        tiles.push_back(row);
    }

    Cluster(Cluster& first, Cluster& second) {
        // assumes west -> east, and that number of rows of tiles match
        //
        if (first.tiles.size() != second.tiles.size()) {
            throw std::runtime_error("invalid sub-cluster dimensions");
        }

        Face first_east = first.east();
        Face second_west = second.west();
        if (first_east != second_west) {
            throw std::runtime_error("invalid cluster border merge");
        }

        int n_rows = first.tiles.size();
        std::vector<Tile> row;
        for (int i = 0; i < n_rows; i++) {
            row.clear();
            // first cluster
            for (int j = 0; j < first.tiles[i].size(); j++) {
                row.push_back(first.tiles[i][j]);
            }
            // second cluster
            for (int j = 0; j < second.tiles[i].size(); j++) {
                row.push_back(second.tiles[i][j]);
            }
        this->tiles.push_back(row);
        }
    }

    friend std::ostream& operator<<(std::ostream&, const Cluster&);

    void print_tiles(void) {
        for (int i = 0; i < tiles.size(); i++) {
            for (int j = 0; j < tiles[0].size(); j++) {
                std::cout << tiles[i][j].id << ' ';
            }
            std::cout << std::endl;
        }
    }

    void rotate_once(void) {
        int n_rows = tiles.size();
        int n_cols = tiles[0].size();
        int new_n_rows = n_cols;
        int new_n_cols = n_rows;
        std::vector<Tile> row;

        std::vector<std::vector<Tile>> new_tiles;
        for (int i = 0; i < new_n_rows; i++) {
            row.clear();
            for (int j = 0; j < new_n_cols; j++) {
                row.push_back(Tile());
            }
            new_tiles.push_back(row);
        }

        // First rotate the tile arrangement.
        for (int i = 0; i < n_rows; i++) {
            for (int j = 0; j < n_cols; j++) {
                new_tiles[j][new_n_cols - 1 - i] = tiles[i][j];
            }
        }

        // Then rotate each tile.
        for (int i = 0; i < new_n_rows; i++) {
            for (int j = 0; j < new_n_cols; j++) {
                new_tiles[i][j].rotate_once();
            }
        }
        this->tiles = new_tiles;
    }

    void flip_ud(void) {
        std::vector<Tile> tmp_tile_row;
        // First flip the arrangement.
        for (int i = 0; i < floor(tiles.size() / 2); i++) {
            tmp_tile_row = tiles[i];
            tiles[i] = tiles[tiles.size() - 1 - i];
            tiles[tiles.size() -1 - i] = tmp_tile_row;
        }
        // Then flip each tile.
        for (int i = 0; i < tiles.size(); i++) {
            for (int j = 0; j < tiles[i].size(); j++) {
                tiles[i][j].flip_ud();
            }
        }

    }
};

std::ostream& operator<<(std::ostream& os, const Cluster& cluster) {
    Tile tile;
    for (int i = 0; i < cluster.tiles.size(); i++) {
        for (int a = 0; a < N; a++) {
            for (int j = 0; j < cluster.tiles[i].size(); j++) {
                for (int b = 0; b < N; b++) {
                    switch (cluster.tiles[i][j].elements[a][b]) {
                        case false:
                            os << '.';
                            break;
                        case true:
                            os << '#';
                            break;
                    }
                }
            }
            os << std::endl;
        }
    }
    return os;
}


int main(int argc, char** argv) {

    std::string line;
    std::ifstream fs(argv[1]);
    std::unordered_map<int, Tile> tiles;

    while (std::getline(fs, line)) {
        if (line.empty()) {
            continue;
        }
        else if (line.substr(0, 4) == "Tile") {
            Tile tile = Tile::from_file(fs, line);
            tiles[tile.id] = tile;
        }
    }
    
    // A nice test case for the south-west corner of the small input.
    Tile first = tiles[2729];
    Tile second = tiles[2971];
    first.rotate_once();
    second.rotate_once();
    Cluster cluster_one(first, second);
    cluster_one.rotate_once();
    cluster_one.flip_ud();
    cluster_one.rotate_once();
    cluster_one.rotate_once();
    std::cout << cluster_one << std::endl;

    Tile third = tiles[1427];
    Tile fourth = tiles[1489];
    third.flip_ud();
    fourth.flip_ud();
    third.rotate(3);
    fourth.rotate(3);
    Cluster cluster_two(third, fourth);
    cluster_two.rotate_once();
    std::cout << cluster_two << std::endl;
    
    Cluster cluster_three(cluster_one, cluster_two);
    std::cout << cluster_three << std::endl;
}
