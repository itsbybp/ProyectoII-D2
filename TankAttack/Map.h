#pragma once
#include "Cell.h"
#include "Tank.h"
#include <vector>

class Map {
public:
    static const int ROWS = 15;
    static const int COLS = 19;
    static const int CELL_SIZE = 50;

    int selectedRow = -1;
    int selectedCol = -1;

    bool isWhite = true;
    int player = 0;
    bool** adjacency;

    Cell grid[ROWS][COLS];
    Tank tanks[4];

    Map();
    ~Map();
    void generate();
    bool isObstacle(int row, int col) const;
    void draw();
    bool change();

private:
    int toIndex(int row, int col) const;
    void buildAdjacency();
    bool isFullyConnected() const;
};