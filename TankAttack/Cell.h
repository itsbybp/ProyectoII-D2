#pragma once

struct Cell {
    bool isObstacle;
    bool hasTank;
    int row;
    int col;

    Cell() : isObstacle(false), row(0), col(0), hasTank(false) {}
    Cell(int r, int c) : isObstacle(false), row(r), col(c), hasTank(false) {}
};