#include <iostream>
#include <fstream>
#include <string>
#include <vector>


using row = std::vector<bool>;
using image = std::vector<row>;


image read_original(std::ifstream& fs, char empty) {
    std::string line;
    image original;
    row tmp;
    while (std::getline(fs, line)) {
        tmp.clear();
        for (auto c : line) {
            if (c == empty)
                tmp.push_back(false);
            else
                tmp.push_back(true);
        }
        original.push_back(tmp);
    }
    return original;
}

void print_image(image im) {
    for (auto r : im) {
        for (auto b : r) {
            switch (b) {
                case false:
                    std::cout << '.';
                    break;
                case true:
                    std::cout << '#';
                    break;
            }
        }
        std::cout << std::endl;
    }
}

void print_size(const image& im) {
    std::cout << "(" << im.size() << ", " << im[0].size() << ")" << std::endl;
}


image trim_borders(image original) {
    image newimage;
    row tmp;
    for (int i = 0; i < original.size(); i++) {
        if (((i % 10) == 9) || ((i % 10) == 0)) {
            continue;
        }
        tmp.clear();
        for (int j = 0; j < original[i].size(); j++) {
            if (((j % 10) == 9) || ((j % 10) == 0)) {
                continue;
            }
            tmp.push_back(original[i][j]);
        }
        newimage.push_back(tmp);
    }
    return newimage;
}

image rotate_image(const image& original) {
    // assume square
    image rotated = original;
    int n = original.size();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            rotated[j][n - 1 - i] = original[i][j];
        }
    }
    return rotated;
}

image flip_ud(const image& original) {
    // assume square
    int n = original.size();
    image flipped = original;
    for (int i = 0; i < n / 2; i++) {
        flipped[i] = original[n - 1 - i];
        flipped[n - 1 - i] = original[i];
    }
    return flipped;
}


bool pattern_match(const image& im, const image& pat, const int start_i, const int start_j) {
    for (int i = 0; i < pat.size(); i++) {
        for (int j = 0; j < pat[i].size(); j++) {
            if (pat[i][j] == false)
                continue;
            if ((pat[i][j] == true) && (im[start_i + i][start_j + j] == false)) {
                return false; 
            }
        }
    }
    return true;
}


void update_mask(image& mask, const image& pat, const int start_i, const int start_j) {
    for (int i = 0; i < pat.size(); i++) {
        for (int j = 0; j < pat[i].size(); j++) {
            if (pat[i][j] == true)
                mask[start_i + i][start_j + j] = true;
        }
    }
}


void convolve_and_update(const image& trimmed, const image& pattern, image& mask) {
    // Convolve pattern over trimmed image.
    int trimmed_height = trimmed.size();
    int trimmed_width = trimmed[0].size();
    int pattern_height = pattern.size();
    int pattern_width = pattern[0].size();

    for (int i = 0; i < trimmed_height - pattern_height; i++) {
        for (int j = 0; j < trimmed_width - pattern_width; j++) {
            if (pattern_match(trimmed, pattern, i, j)) {
                update_mask(mask, pattern, i, j);
            }
        }
    }
}


int count_unmasked_hashes(const image& trimmed, const image& mask) {
    int sum = 0;
    for (int i = 0; i < trimmed.size(); i++) {
        for (int j = 0; j < trimmed[0].size(); j++) {
            if ((trimmed[i][j] == true) && (mask[i][j] == false))
                sum++;
        }
    }
    return sum;
}

int main(int argc, char** argv) {

    // Read image from part 1.
    std::ifstream fs(argv[1]);
    image original = read_original(fs, '.');

    // Read monster pattern.
    fs.close();
    fs = std::ifstream(argv[2]);
    image pattern = read_original(fs, ' ');
    print_image(pattern);

    // Trim borders.
    image trimmed = trim_borders(original);
    std::cout << trimmed.size() << " rows." << std::endl;
    std::cout << trimmed[0].size() << " columns." << std::endl;

    // Prepare mask.
    image mask = trimmed;
    for (int i = 0; i < mask.size(); i++)
        for (int j = 0; j < mask[i].size(); j++) 
            mask[i][j] = false;


    // Convolve in each orientation, updating mask.
    // N
    std::cout << "convolve N" << std::endl;
    convolve_and_update(trimmed, pattern, mask);

    // E
    trimmed = rotate_image(trimmed);
    mask = rotate_image(mask);
    std::cout << "convolve E" << std::endl;
    convolve_and_update(trimmed, pattern, mask);

    // S
    trimmed = rotate_image(trimmed);
    mask = rotate_image(mask);
    std::cout << "convolve S" << std::endl;
    convolve_and_update(trimmed, pattern, mask);

    // W
    trimmed = rotate_image(trimmed);
    mask = rotate_image(mask);
    std::cout << "convolve W" << std::endl;
    convolve_and_update(trimmed, pattern, mask);

    std::cout << "FLIP!" << std::endl;
    trimmed = flip_ud(trimmed);
    mask = flip_ud(mask);

    // N
    std::cout << "convolve N" << std::endl;
    convolve_and_update(trimmed, pattern, mask);

    // E
    trimmed = rotate_image(trimmed);
    mask = rotate_image(mask);
    std::cout << "convolve E" << std::endl;
    convolve_and_update(trimmed, pattern, mask);

    // S
    trimmed = rotate_image(trimmed);
    mask = rotate_image(mask);
    std::cout << "convolve S" << std::endl;
    convolve_and_update(trimmed, pattern, mask);

    // W
    trimmed = rotate_image(trimmed);
    mask = rotate_image(mask);
    std::cout << "convolve W" << std::endl;
    convolve_and_update(trimmed, pattern, mask);

    // Put back to original orientation.
    trimmed = rotate_image(trimmed);
    mask = rotate_image(mask);
    trimmed = flip_ud(trimmed);
    mask = flip_ud(mask);

    print_image(mask);
    
    std::cout << "Roughness = " << count_unmasked_hashes(trimmed, mask) << std::endl;

}
