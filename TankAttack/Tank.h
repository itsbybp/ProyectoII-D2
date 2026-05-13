#pragma once
#include "raylib.h"
#include "Cell.h"
#include "PowerUp.h"
#include <cstdlib>

class Map;

struct Tank {
    int row;
    int col;
    int user;
    int health;
    Color color;
    PowerUpQueue powerUps;

    // Estados de power-up activo
    bool precisionMoveAct = false;
    bool precisionAtckAct = false;
    bool attackPwrAct = false;
    int doubleTurn = 0;

    Tank() : row(0), col(0), user(0), health(100), color(BLACK) {}
    Tank(int r, int c, int u, int h, Color col) : row(r), col(c), user(u), health(h), color(col) {}

    void updatePosition(int user, Cell& current, Cell& target);
    void moveBFS(int user, Map& map, Cell& start, Cell& target);
    void moveDijkstra(int user, Map& map, Cell& start, Cell& target);
    void moveRandom(int user, Map& map, Cell& start, Cell& target);
};