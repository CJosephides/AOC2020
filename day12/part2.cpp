#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

struct Position {
    int x;
    int y;
};


class Ship {
    public:
        Position center;
        Position waypoint;

    Ship() :center({0,0}), waypoint({10,1}) {};  // default starting positions

    void navigate(char instruction, int magnitude) {
        if (instruction == 'N') waypoint.y += magnitude;
        else if (instruction == 'E') waypoint.x += magnitude;
        else if (instruction == 'S') waypoint.y -= magnitude;
        else if (instruction == 'W') waypoint.x -= magnitude;
        else if (instruction == 'F') move_forward(magnitude);
        else if ((instruction == 'L') or (instruction == 'R')) {
            rotate_waypoint(instruction == 'L' ? magnitude : 360 - magnitude);
        }
    }

    void move_forward(int magnitude) {
        int delta_x = waypoint.x - center.x;
        int delta_y = waypoint.y - center.y;
        center.x += magnitude * (delta_x);
        center.y += magnitude * (delta_y);
        waypoint.x += magnitude * (delta_x);
        waypoint.y += magnitude * (delta_y);
    }

    void rotate_waypoint(int theta) {
        // Assumes counter clock wise rotation by theta.
        int delta_x = waypoint.x - center.x;
        int delta_y = waypoint.y - center.y;
        if (theta == 90) {
            waypoint.x = center.x - delta_y;
            waypoint.y = center.y + delta_x;
        } else if (theta == 180) {
            waypoint.x = center.x - delta_x;
            waypoint.y = center.y - delta_y;
        } else if (theta == 270) {
            waypoint.x = center.x + delta_y;
            waypoint.y = center.y - delta_x;
        } else throw std::runtime_error("invalid rotation");
    }

    friend std::ostream& operator<<(std::ostream&, const Ship&);
};

std::ostream& operator<<(std::ostream& os, const Ship& ship) {
    os << "ship: {x = " << ship.center.x << ", y = " << ship.center.y << "} ";
    os << "waypoint: {x = " << ship.waypoint.x << ", y = " << ship.waypoint.y << "}";
    return os << '\n';

}

int main(int argc, char** argv) {

    std::ifstream filestream(argv[1]);
    if (!filestream.is_open()) {
        std::cout << "could not open file " << argv[1] << " for reading.\n";
    }

    Ship ship = Ship();
    std::cout << "Starting navigation.\n";
    std::cout << ship;

    std::string line;
    char instruction;
    int magnitude;
    while (std::getline(filestream, line)) {
        instruction = line[0];
        magnitude = std::atoi(line.substr(1, std::string::npos).c_str());
        ship.navigate(instruction, magnitude);
        std::cout << ship;
    }

}
