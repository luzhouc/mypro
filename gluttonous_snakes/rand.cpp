#include <iostream>
#include <random>
#include <unordered_set>

struct Coordinate {
    int x;
    int y;
};

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> xDist(1, 20);
    std::uniform_int_distribution<int> yDist(1, 15);

    const int numCoordinates = 15;
    std::unordered_set<int> generatedCoordinates;
    Coordinate coordinates[numCoordinates];

    for (int i = 0; i < numCoordinates; ++i) {
        Coordinate newCoordinate;
        do {
            newCoordinate.x = xDist(gen);
            newCoordinate.y = yDist(gen);
        } while (generatedCoordinates.count(newCoordinate.x * 100 + newCoordinate.y) > 0);

        generatedCoordinates.insert(newCoordinate.x * 100 + newCoordinate.y);
        coordinates[i] = newCoordinate;
    }

    // 输出生成的坐标
    for (int i = 0; i < numCoordinates; ++i) {
        std::cout << "坐标 " << i + 1 << ": (" << coordinates[i].x << ", " << coordinates[i].y << ")\n";
    }

    return 0;
}
