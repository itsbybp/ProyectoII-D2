#pragma once
#include "Cell.h"
#include "Tank.h"
#include <cstdio>

class Map {
public:
    static const int ROWS = 15;
    static const int COLS = 19;
    static const int CELL_SIZE = 50;
    static const int BAR_HEIGHT = 60;
    void buildAdjacency();
    void buildAdjacencyForTank(int tankIdx);

    int selectedRow = -1;
    int selectedCol = -1;
    int highlightRow = -1;
    int highlightCol = -1;
    bool isSelected = false;
    int selectedTank = -1;
    int player = 0;
    bool moving = false;

    // 0=ninguno, 1=mover, 2=disparar, 3=powerup
    int actionMode = 0;

    // Ruta de movimiento (array manual)
    Cell* path[ROWS * COLS];
    int pathSize = 0;

    // Trazo de bala
    Cell* bulletPath[ROWS * COLS * 4];
    int bulletPathSize = 0;
    bool showBulletPath = false;

    bool** adjacency;
    Cell grid[ROWS][COLS];
    Tank tanks[4];

    Map();
    ~Map();
    void generate();
    void updateMovement();
    bool isObstacle(int row, int col) const;
    void draw();
    bool change();
    void shootBullet(int fromRow, int fromCol, int dirRow, int dirCol, int tankIdx);

private:
    int toIndex(int row, int col) const;
    bool isFullyConnected() const;
    void drawHUD();
    void drawActionButtons();
};