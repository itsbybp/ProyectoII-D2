#pragma once

struct Cell {
    bool isObstacle;
    int row;
    int col;

    Cell() : isObstacle(false), row(0), col(0) {}
    Cell(int r, int c) : isObstacle(false), row(r), col(c) {}
};