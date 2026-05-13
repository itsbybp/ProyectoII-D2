#include "Tank.h"
#include "Map.h"
#include "Cell.h"
#include "raylib.h"
#include <cstdlib>

void Tank::updatePosition(int user, Cell& current, Cell& target) {
    if (user == 0) {
        current.hasTank0 = false;
        target.hasTank0 = true;
    }
    else {
        current.hasTank1 = false;
        target.hasTank1 = true;
    }
    row = target.row;
    col = target.col;
}

void Tank::moveBFS(int user, Map& map, Cell& start, Cell& target) {
    map.pathSize = 0;
    int total = Map::ROWS * Map::COLS;
    int stIdx = start.row * Map::COLS + start.col;
    int tgIdx = target.row * Map::COLS + target.col;

    int* parent = new int[total];
    bool* visited = new bool[total];
    for (int i = 0; i < total; i++) { parent[i] = -1; visited[i] = false; }

    int* q = new int[total];
    int front = 0, back = 0;
    q[back++] = stIdx;
    visited[stIdx] = true;

    while (front < back) {
        int curr = q[front++];
        if (curr == tgIdx) break;
        for (int j = 0; j < total; j++) {
            if (map.adjacency[curr][j] && !visited[j]) {
                visited[j] = true;
                parent[j] = curr;
                q[back++] = j;
            }
        }
    }

    int* tmp = new int[total];
    int tmpSize = 0;
    int current = tgIdx;
    while (current != -1) { tmp[tmpSize++] = current; current = parent[current]; }
    for (int i = tmpSize - 1; i >= 0; i--) {
        int idx = tmp[i];
        map.path[map.pathSize++] = &map.grid[idx / Map::COLS][idx % Map::COLS];
    }

    map.moving = true;
    map.showBulletPath = false;
    delete[] parent; delete[] visited; delete[] q; delete[] tmp;
}

void Tank::moveDijkstra(int user, Map& map, Cell& start, Cell& target) {
    map.pathSize = 0;
    int total = Map::ROWS * Map::COLS;
    int startIdx = start.row * Map::COLS + start.col;
    int targetIdx = target.row * Map::COLS + target.col;

    int* dist = new int[total];
    int* parent = new int[total];
    bool* visited = new bool[total];
    for (int i = 0; i < total; i++) { dist[i] = 99999; parent[i] = -1; visited[i] = false; }
    dist[startIdx] = 0;

    for (int i = 0; i < total; i++) {
        int curr = -1, minDist = 99999;
        for (int j = 0; j < total; j++)
            if (!visited[j] && dist[j] < minDist) { minDist = dist[j]; curr = j; }
        if (curr == -1) break;
        visited[curr] = true;
        for (int j = 0; j < total; j++) {
            if (map.adjacency[curr][j] && !visited[j]) {
                int nd = dist[curr] + 1;
                if (nd < dist[j]) { dist[j] = nd; parent[j] = curr; }
            }
        }
    }

    int* tmp = new int[total];
    int tmpSize = 0;
    int current = targetIdx;
    while (current != -1) { tmp[tmpSize++] = current; current = parent[current]; }
    for (int i = tmpSize - 1; i >= 0; i--) {
        int idx = tmp[i];
        map.path[map.pathSize++] = &map.grid[idx / Map::COLS][idx % Map::COLS];
    }

    map.moving = true;
    map.showBulletPath = false;
    delete[] dist; delete[] parent; delete[] visited; delete[] tmp;
}

void Tank::moveRandom(int user, Map& map, Cell& start, Cell& target) {
    map.pathSize = 0;
    map.path[map.pathSize++] = &map.grid[row][col];

    const int radius = 5;
    int currRow = row, currCol = col;
    int dRow = (target.row > currRow) ? 1 : (target.row < currRow) ? -1 : 0;
    int dCol = (target.col > currCol) ? 1 : (target.col < currCol) ? -1 : 0;

    bool blocked = false;
    while (currRow != target.row || currCol != target.col) {
        int nextRow = currRow, nextCol = currCol;
        if (currRow != target.row) nextRow += dRow;
        else nextCol += dCol;

        if (nextRow < 0 || nextRow >= Map::ROWS || nextCol < 0 || nextCol >= Map::COLS ||
            map.grid[nextRow][nextCol].isObstacle ||
            map.grid[nextRow][nextCol].hasTank0 ||
            map.grid[nextRow][nextCol].hasTank1) {
            blocked = true; break;
        }
        map.path[map.pathSize++] = &map.grid[nextRow][nextCol];
        currRow = nextRow; currCol = nextCol;
    }

    if (!blocked) { map.moving = true; map.showBulletPath = false; return; }

    int randRow = currRow, randCol = currCol;
    int attempts = 0;
    bool valid = false;
    while (!valid && attempts < 100) {
        randRow = currRow + (rand() % (2 * radius + 1)) - radius;
        randCol = currCol + (rand() % (2 * radius + 1)) - radius;
        if (randRow >= 0 && randRow < Map::ROWS && randCol >= 0 && randCol < Map::COLS &&
            !map.grid[randRow][randCol].isObstacle &&
            !map.grid[randRow][randCol].hasTank0 &&
            !map.grid[randRow][randCol].hasTank1)
            valid = true;
        attempts++;
    }

    while (currRow != randRow || currCol != randCol) {
        int nextRow = currRow, nextCol = currCol;
        if (currRow < randRow) nextRow++;
        else if (currRow > randRow) nextRow--;
        else if (currCol < randCol) nextCol++;
        else nextCol--;
        if (map.grid[nextRow][nextCol].isObstacle ||
            map.grid[nextRow][nextCol].hasTank0 ||
            map.grid[nextRow][nextCol].hasTank1) break;
        map.path[map.pathSize++] = &map.grid[nextRow][nextCol];
        currRow = nextRow; currCol = nextCol;
    }

    dRow = (target.row > currRow) ? 1 : (target.row < currRow) ? -1 : 0;
    dCol = (target.col > currCol) ? 1 : (target.col < currCol) ? -1 : 0;
    while (currRow != target.row || currCol != target.col) {
        int nextRow = currRow, nextCol = currCol;
        if (currRow != target.row) nextRow += dRow;
        else nextCol += dCol;
        if (nextRow < 0 || nextRow >= Map::ROWS || nextCol < 0 || nextCol >= Map::COLS ||
            map.grid[nextRow][nextCol].isObstacle ||
            map.grid[nextRow][nextCol].hasTank0 ||
            map.grid[nextRow][nextCol].hasTank1) break;
        map.path[map.pathSize++] = &map.grid[nextRow][nextCol];
        currRow = nextRow; currCol = nextCol;
    }

    map.moving = true;
    map.showBulletPath = false;
}