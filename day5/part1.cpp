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
};


int main(int argc, char** argv) {
    ifstream filestream = ifstream(argv[1]);
    string line;
    int highest_uid = 0;
    while (getline(filestream, line, '\n')) {
        cout << line << "\n";
        Seat seat = Seat(line);
        cout << "Row = " << seat.row << "; Column = " << seat.column << "; UID = " << seat.uid << '\n';
        highest_uid = max({highest_uid, seat.uid});
    }
    cout << "Higest unique ID = " << highest_uid << ".\n";
}
