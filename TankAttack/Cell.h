#pragma once

struct Cell {
    bool isObstacle;
    bool hasTank0;
	bool hasTank1;
    int row;
    int col;

    Cell() : isObstacle(false), row(0), col(0), hasTank0(false), hasTank1(false) {}
    Cell(int r, int c) : isObstacle(false), row(r), col(c), hasTank0(false), hasTank1(false) {}
};