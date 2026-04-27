#pragma once
#include "Cell.h"

class Map {
public:
    static const int ROWS = 15;
    static const int COLS = 19;
    static const int CELL_SIZE = 50;
    bool isWhite = true;

    Map();
    ~Map();
    void generate();
    bool isObstacle(int row, int col) const;
    void draw();
	void redraw();
    bool change();

private:
    Cell grid[ROWS][COLS];
    bool** adjacency;

    int toIndex(int row, int col) const;
    void buildAdjacency();
    bool isFullyConnected() const;
};