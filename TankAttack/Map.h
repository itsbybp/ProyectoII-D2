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

    int highlightRow = -1;
	int highlightCol = -1;

	int isSelected = false;
    int selectedTank = -1;

    bool isWhite = true;
    bool moving = false;
    int player = 0;
    bool** adjacency;

    std::vector<Cell*> path;

    Cell grid[ROWS][COLS];
    Tank tanks[4];

    Map();
    ~Map();
    void generate();
	void updateMovement();
    bool isObstacle(int row, int col) const;
    void draw();
    bool change();
    void quickUpdate(int user, Cell*& currentCell, Cell*& nextCell);

private:
    int toIndex(int row, int col) const;
    void buildAdjacency();
    bool isFullyConnected() const;
};