#include <iostream>
#include <fstream>
#include <string>


enum class Direction { north, east, south, west };

Direction& operator++(Direction& direction) {

    switch (direction) {
        case Direction::north:
            return direction = Direction::east;
        case Direction::east:
            return direction = Direction::south;
        case Direction::south:
            return direction = Direction::west;
        case Direction::west:
            return direction = Direction::north;
    }
}

Direction& operator--(Direction& direction) {

    switch (direction) {
        case Direction::north:
            return direction = Direction::west;
        case Direction::east:
            return direction = Direction::north;
        case Direction::south:
            return direction = Direction::east;
        case Direction::west:
            return direction = Direction::south;
    }
}

std::ostream& operator<<(std::ostream& os, const Direction& direction) {
    switch (direction) {
        case Direction::north:
            return os << "north";
        case Direction::east:
            return os << "east";
        case Direction::south:
            return os << "south";
        case Direction::west:
            return os << "west";
    }
}

enum Bearing { clockwise, anticlockwise };


struct Position {
    int x;
    int y;
};

class Ship {
    public:
        Direction orientation;
        Position position;

    void navigate(char instruction, int magnitude) {
        if (instruction == 'N') position.y += magnitude;
        else if (instruction == 'E') position.x += magnitude;
        else if (instruction == 'S') position.y -= magnitude;
        else if (instruction == 'W') position.x -= magnitude;
        else if (instruction == 'F') move_forward(magnitude);
        else if ((instruction == 'L') or (instruction == 'R')) {
            change_orientation(instruction == 'L' ? Bearing::anticlockwise : Bearing::clockwise, magnitude);
        }
    }

    Ship() {
        orientation  = Direction::east;
        position = Position { 0, 0 };
    };

    void change_orientation(Bearing bearing , int magnitude) {
        int increments = magnitude / 90;
        switch (bearing) {
            case Bearing::anticlockwise:
                for (int i = 0; i < increments; i++) --orientation;
                break;
            case Bearing::clockwise:
                for (int i = 0; i < increments; i++) ++orientation;
                break;
        }
    }

    void move_forward(int magnitude) {
        switch (orientation) {
            case Direction::north:
                position.y += magnitude;
                break;
            case Direction::east:
                position.x += magnitude;
                break;
            case Direction::south:
                position.y -= magnitude;
                break;
            case Direction::west:
                position.x -= magnitude;
                break;
        }
    }

    friend std::ostream& operator<<(std::ostream&, const Ship&);
};


std::ostream& operator<<(std::ostream& os, const Ship& ship) {
    return os << "{x = " << ship.position.x << ", y = " << ship.position.y << "} --- (" << ship.orientation << ").\n";
}

int main(int argc, char** argv) {

    std::ifstream filestream(argv[1]);
    if (!filestream.is_open()) {
        std::cout << "could not open file " << argv[1] << " for reading.\n";
    }

    Ship ship = Ship();
    std::cout << "Starting navigation. Ship at " << ship << ".\n";

    std::string line;
    char instruction;
    int magnitude;
    while (std::getline(filestream, line)) {
        instruction = line[0];
        magnitude = std::atoi(line.substr(1, std::string::npos).c_str());
        ship.navigate(instruction, magnitude);
        std::cout << "Ship at " << ship << ".\n";
    }

}
