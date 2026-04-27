#pragma once
#include "Cell.h"

class Map {
public:
    static const int ROWS = 15;
    static const int COLS = 19;
    static const int CELL_SIZE = 50;

    Map();
    ~Map();
    void generate();
    bool isObstacle(int row, int col) const;
    void draw() const;

private:
    Cell grid[ROWS][COLS];
    bool** adjacency;

    int toIndex(int row, int col) const;
    void buildAdjacency();
    bool isFullyConnected() const;
};