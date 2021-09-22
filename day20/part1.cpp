#include <iostream>
#include <fstream>
#include <unordered_map>
#include <array>
#include <vector>
#include <string>
#include <stdexcept>
#include <cmath>
#include <utility>


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

    bool operator!=(const Tile& other) {
        return !(*this == other);
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
    Cluster(void) {};

    Cluster(Tile& first, Tile& second) {
        // assumes west -> east
        Face first_east = first.east();
        Face second_west = second.west();
        if (first_east != second_west) {
            throw std::runtime_error("invalid tile border merge");
        }
        std::vector<Tile> row;
        row.push_back(first);
        row.push_back(second);
        tiles.push_back(row);
    }

    Cluster(Cluster& first, Tile& second) {
        // assumes west -> east
        //
        if (first.tiles.size() != 1) {
            throw std::runtime_error("invalid sub-cluster dimensions");
        }

        Face first_east = first.east();
        Face second_west = second.west();
        if (first_east != second_west) {
            throw std::runtime_error("invalid tile border merge");
        }
        std::vector<Tile> row;
        for (auto tile : first.tiles[0]) {
            row.push_back(tile);
        }
        row.push_back(second);
        tiles.push_back(row);
    }

    Cluster(Cluster& first, Cluster& second) {
        // assumes west -> east, and that number of rows of tiles match
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

    static bool valid_merge(Tile& first, Tile& second) {
        if (first.id == second.id)
            return false;
        Face first_east = first.east();
        Face second_west = second.west();
        return first_east == second_west;
    }

    static bool valid_merge(Cluster& first, Tile& second) {
        for (auto first_tile_row : first.tiles)
            for (auto first_tile : first_tile_row)
                if (first_tile.id == second.id)
                    return false;  // clusters can't contain the same tile id!
        Face first_east = first.east();
        Face second_west = second.west();
        return ((first.tiles.size() == 1) && (first_east == second_west));
    }

    static bool valid_merge(Cluster& first, Cluster& second) {
        for (auto first_tile_row : first.tiles)
            for (auto first_tile : first_tile_row)
                for (auto second_tile_row : second.tiles)
                    for (auto second_tile : second_tile_row)
                        if (first_tile.id == second_tile.id)
                    return false;  // clusters can't contain the same tile id!
        Face first_east = first.east();
        Face second_west = second.west();
        return ((first.tiles.size() == second.tiles.size()) && (first_east == second_west));
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

    std::pair<int, int> shape(void) {
        std::pair<int, int> s = {tiles.size(), tiles[0].size()};
        return s;
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

bool operator==(Cluster& first, Cluster& second) {
    // Check if all tile IDs match.
    if (first.tiles.size() != second.tiles.size())
        return false;
    if (first.tiles[0].size() != second.tiles[0].size())
        return false;
    for (int i = 0; i < first.tiles.size(); i++) {
        for (int j = 0; j < first.tiles[i].size(); j++) {
            if (first.tiles[i][j] != second.tiles[i][j])
                return false;
        }
    }
    return true;
    // TODO also check elements.
}


bool operator!=(Cluster& first, Cluster& second) {
    return (!(first == second));
}


bool rotational_equivalence(Cluster& first, Cluster& second) {
    Cluster tmp = first;
    if (tmp == second)
        return true;
    tmp.rotate_once();
    if (tmp == second)
        return true;
    tmp.rotate_once();
    if (tmp == second)
        return true;
    tmp.rotate_once();
    if (tmp == second)
        return true;
    tmp.flip_ud();
    if (tmp == second)
        return true;
    tmp.rotate_once();
    if (tmp == second)
        return true;
    tmp.rotate_once();
    if (tmp == second)
        return true;
    tmp.rotate_once();
    if (tmp == second)
        return true;
    return false;
}


std::vector<Tile> all_orientations(Tile& tile, bool flippy) {

    std::vector<Tile> orientations;
    orientations.push_back(tile);

    tile.rotate_once();
    orientations.push_back(tile);

    tile.rotate_once();
    orientations.push_back(tile);

    tile.rotate_once();
    orientations.push_back(tile);
    tile.rotate_once();

    if (flippy) {
        tile.flip_ud();
        orientations.push_back(tile);

        tile.rotate_once();
        orientations.push_back(tile);

        tile.rotate_once();
        orientations.push_back(tile);

        tile.rotate_once();
        orientations.push_back(tile);

        tile.rotate_once();
        tile.flip_ud();
    }

    return orientations;
}

std::vector<Cluster> all_orientations(Cluster& cluster, bool flippy) {
    std::vector<Cluster> orientations;
    orientations.push_back(cluster);

    cluster.rotate_once();
    orientations.push_back(cluster);

    cluster.rotate_once();
    orientations.push_back(cluster);

    cluster.rotate_once();
    orientations.push_back(cluster);
    cluster.rotate_once();

    if (flippy) {
        cluster.flip_ud();
        orientations.push_back(cluster);

        cluster.rotate_once();
        orientations.push_back(cluster);

        cluster.rotate_once();
        orientations.push_back(cluster);

        cluster.rotate_once();
        orientations.push_back(cluster);

        cluster.rotate_once();
        cluster.flip_ud();
    }

    return orientations;
}


std::vector<Cluster> all_pairwise_mergers(Tile& first, Tile& second) {
    std::vector<Cluster> all_clusters;
    std::vector<Tile> all_first = all_orientations(first, true);
    std::vector<Tile> all_second = all_orientations(second, false);
    Cluster merged_cluster;
    for (auto fo : all_first) {
        for (auto so : all_second) {
            if (Cluster::valid_merge(fo, so)) {
                merged_cluster = Cluster(fo, so);
                bool add_cluster = true;
                for (auto existing_cluster : all_clusters) {
                    add_cluster &= (existing_cluster != merged_cluster);
                    if (!add_cluster)
                        break;
                }
                if (add_cluster)
                    all_clusters.push_back(merged_cluster);
            }
        }
    }
    return all_clusters;
}

std::vector<Cluster> all_pairwise_mergers(Cluster& first, Tile& second) {
    std::vector<Cluster> all_clusters;
    std::vector<Cluster> all_first = all_orientations(first, true);
    std::vector<Tile> all_second = all_orientations(second, false);
    Cluster merged_cluster;
    for (auto fo : all_first) {
        for (auto so : all_second) {
            if (Cluster::valid_merge(fo, so)) {
                merged_cluster = Cluster(fo, so);
                bool add_cluster = true;
                for (auto existing_cluster : all_clusters) {
                    add_cluster &= (existing_cluster != merged_cluster);
                    if (!add_cluster)
                        break;
                }
                if (add_cluster)
                    all_clusters.push_back(merged_cluster);
            }
        }
    }
    return all_clusters;
}

std::vector<Cluster> all_pairwise_mergers(Cluster& first, Cluster& second) {
    std::vector<Cluster> all_clusters;
    std::vector<Cluster> all_first = all_orientations(first, true);
    std::vector<Cluster> all_second = all_orientations(second, false);
    Cluster merged_cluster;
    for (auto fo : all_first) {
        for (auto so : all_second) {
            if (Cluster::valid_merge(fo, so)) {
                merged_cluster = Cluster(fo, so);
                bool add_cluster = true;
                for (auto existing_cluster : all_clusters) {
                    add_cluster &= (existing_cluster != merged_cluster);
                    if (!add_cluster)
                        break;
                }
                if (add_cluster)
                    all_clusters.push_back(merged_cluster);
            }
        }
    }
    return all_clusters;
}


std::vector<Cluster> merge_cluster_vectors(std::vector<Tile>& first_vector, std::vector<Tile>& second_vector) {

    bool add_cluster;
    std::vector<Cluster> merged_vector;
    for (auto cluster : first_vector) {
        for (auto other_cluster : second_vector) {
            for (auto merged_cluster : all_pairwise_mergers(cluster, other_cluster)) {
                add_cluster = true;
                for (auto existing_cluster : merged_vector) {
                    add_cluster &= (merged_cluster != existing_cluster);
                    if (!add_cluster)
                        break;
                }
                if (add_cluster)
                    merged_vector.push_back(merged_cluster);
            }
        }
    }
    return merged_vector;
}


std::vector<Cluster> merge_cluster_vectors(std::vector<Cluster>& first_vector, std::vector<Tile>& second_vector,
                                           std::pair<int, int> target_shape) {

    bool add_cluster;
    std::vector<Cluster> merged_vector;
    for (auto cluster : first_vector) {
        for (auto other_cluster : second_vector) {
            for (auto merged_cluster : all_pairwise_mergers(cluster, other_cluster)) {
                add_cluster = true;
                for (auto existing_cluster : merged_vector) {
                    add_cluster &= (merged_cluster != existing_cluster);
                    if (!add_cluster)
                        break;
                }
                if (add_cluster) { 
                    if ((merged_cluster.shape().first == target_shape.first) && (merged_cluster.shape().second == target_shape.second))
                        merged_vector.push_back(merged_cluster);
                    merged_vector.push_back(merged_cluster);
                }
            }
        }
    }
    return merged_vector;
}


std::vector<Cluster> merge_cluster_vectors(std::vector<Cluster>& first_vector, std::vector<Cluster>& second_vector,
                                           std::pair<int, int> target_shape) {

    bool add_cluster;
    std::vector<Cluster> merged_vector;
    for (auto cluster : first_vector) {
        for (auto other_cluster : second_vector) {
            for (auto merged_cluster : all_pairwise_mergers(cluster, other_cluster)) {
                add_cluster = true;
                for (auto existing_cluster : merged_vector) {
                    //add_cluster &= (merged_cluster != existing_cluster);
                    add_cluster &= (!rotational_equivalence(merged_cluster, existing_cluster));
                    if (!add_cluster)
                        break;
                }
                if (add_cluster) {
                    if ((merged_cluster.shape().first == target_shape.first) && (merged_cluster.shape().second == target_shape.second))
                        merged_vector.push_back(merged_cluster);
                }
            }
        }
    }
    return merged_vector;
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

    /*
    // Test the small input case.
    std::vector<Tile> one_by_ones;
    for (auto [id, tile]: tiles)
        one_by_ones.push_back(tile);

    std::vector<Cluster> one_by_twos = merge_cluster_vectors(one_by_ones, one_by_ones);
    std::cout << one_by_twos.size() << " 1x2 clusters." << std::endl;

    std::vector<Cluster> one_by_threes = merge_cluster_vectors(one_by_twos, one_by_ones);
    std::cout << one_by_threes.size() << " 1x3 clusters." << std::endl;

    std::vector<Cluster> three_by_twos = merge_cluster_vectors(one_by_threes, one_by_threes, {3, 2});
    std::cout << three_by_twos.size() << " 3x2 clusters." << std::endl;

    std::vector<Cluster> three_by_threes = merge_cluster_vectors(three_by_twos, one_by_threes, {3, 3});
    std::cout << three_by_threes.size() << " 3x3 clusters." << std::endl;

    for (auto cluster : three_by_threes) {
        cluster.print_tiles();
        std::cout << std::endl;
    }
    */

    // Solve full case!
    std::vector<Tile> one_by_ones;
    for (auto [id, tile]: tiles)
        one_by_ones.push_back(tile);

    // 1x1 + 1x1 -> 1x2
    std::vector<Cluster> one_by_twos = merge_cluster_vectors(one_by_ones, one_by_ones);
    std::cout << one_by_twos.size() << " 1x2." << std::endl;

    // 1x2 + 1x2 -> 2x2
    std::vector<Cluster> two_by_twos = merge_cluster_vectors(one_by_twos, one_by_twos, {2, 2});
    std::cout << two_by_twos.size() << " 2x2." << std::endl;

    // 2x2 + 2x2 -> 2x4
    std::vector<Cluster> two_by_fours = merge_cluster_vectors(two_by_twos, two_by_twos, {2, 4});
    std::cout << two_by_fours.size() << " 2x4." << std::endl;

    // 2x4 + 2x4 -> 4x4
    std::vector<Cluster> four_by_fours = merge_cluster_vectors(two_by_fours, two_by_fours, {4, 4});
    std::cout << four_by_fours.size() << " 4x4." << std::endl;

    // 4x4 + 4x4 -> 4x8
    std::vector<Cluster> four_by_eights = merge_cluster_vectors(four_by_fours, four_by_fours, {4, 8});  // w->e
    std::cout << four_by_eights.size() << " 4x8." << std::endl;

    // 4x8 + 4x4 -> 4x12
    std::vector<Cluster> four_by_twelves = merge_cluster_vectors(four_by_eights, four_by_fours, {4, 12});  // w->e
    std::cout << four_by_twelves.size() << " 4x12." << std::endl;

    // 4x12 + 4x12 -> 12x8
    std::vector<Cluster> eight_by_twelves = merge_cluster_vectors(four_by_twelves, four_by_twelves, {12, 8});  // w->e
    std::cout << eight_by_twelves.size() << " 12x8." << std::endl;

    // 8x12 + 4x12 -> 12x12
    std::vector<Cluster> twelve_by_twelves = merge_cluster_vectors(eight_by_twelves, four_by_twelves, {12, 12});  // w->e
    std::cout << twelve_by_twelves.size() << " 12x12." << std::endl;

    std::cout << twelve_by_twelves[0] << std::endl;
    twelve_by_twelves[0].print_tiles();
}
