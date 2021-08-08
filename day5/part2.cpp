#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cmath>

using namespace std;


int binary_partition(const string& s, const int low) {
    // Given a sequence of characters, either `low` or `high` (implicit), return the integer corresponding to the
    // binary partition.
    int n = s.size();
    int lo = 0;
    int hi = pow(2, n) - 1;

    for (auto i = s.begin(); i != s.end(); i++) {
        if (*i == low) {
            hi -= (hi + 1 - lo) / 2;
        } else {
            lo += (hi + 1 - lo) / 2;
        }
        if (lo == hi) return lo;
    }
    return -1;
}


class Seat {
    public:
        int row;
        int column;
        int uid;
        Seat(string line) {
            row = binary_partition(line.substr(0, 7), 'F');
            column = binary_partition(line.substr(7, 3), 'L');
            uid = (row * 8) + column;
        }
        Seat(int row, int column) {
            this->row = row;
            this->column = column;
            this->uid = (row * 8) + column;
        }
};


void idx_to_ij(const int idx, int& i, int& j) {
    i = idx / 8;
    j = idx % 8;
}


//Seat missing_seat(bool (&allocated_seats)[128][8]) {
//Seat missing_seat(bool allocated_seats[128][8]) {
Seat missing_seat(bool allocated_seats[][8]) {
    int idx = 0;
    int i = 0;
    int j = 0;
    while (!allocated_seats[i][j]) {
        idx++;
        idx_to_ij(idx, i, j);
    }
    while (allocated_seats[i][j]) {
        idx++;
        idx_to_ij(idx, i, j);
    }
    return Seat(i, j);
}

int main(int argc, char** argv) {
    ifstream filestream = ifstream(argv[1]);
    string line;
    bool allocated_seats[128][8];
    for (int i = 0; i < 128; i++) {
        for (int j = 0; j < 8; j++) {
            allocated_seats[i][j] = false;
        }
    }
    while (getline(filestream, line, '\n')) {
        cout << line << "\n";
        Seat seat = Seat(line);
        cout << "Row = " << seat.row << "; Column = " << seat.column << "; UID = " << seat.uid << '\n';
        allocated_seats[seat.row][seat.column] = true;
    }
    Seat found = missing_seat(allocated_seats);
    cout << "Missing seat at row = " << found.row << ", column = " << found.column << ". ID = " << (found.row * 8) + found.column << ".\n";
}
