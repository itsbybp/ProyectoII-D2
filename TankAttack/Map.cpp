#include "Map.h"
#include "raylib.h"
#include <cstdlib>
#include <ctime>
#include <queue>

Map::Map() {
    int total = ROWS * COLS;
    adjacency = new bool* [total];
    for (int i = 0; i < total; i++) {
        adjacency[i] = new bool[total];
        for (int j = 0; j < total; j++)
            adjacency[i][j] = false;
    }
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            grid[r][c] = Cell(r, c);
}

Map::~Map() {
    int total = ROWS * COLS;
    for (int i = 0; i < total; i++)
        delete[] adjacency[i];
    delete[] adjacency;
}

int Map::toIndex(int row, int col) const {
    return row * COLS + col;
}

bool Map::isObstacle(int row, int col) const {
    return grid[row][col].isObstacle;
}

void Map::generate() {
    srand((unsigned int)time(nullptr));
    do {
        for (int r = 0; r < ROWS; r++)
            for (int c = 0; c < COLS; c++)
                grid[r][c].isObstacle = false;
        for (int r = 0; r < ROWS; r++)
            for (int c = 0; c < COLS; c++)
                if (rand() % 100 < 30)
                    grid[r][c].isObstacle = true;
        buildAdjacency();
    } while (!isFullyConnected());

    int i = 0;
    while (i < 4) {
        int r = rand() % ROWS;
        int c = rand() % COLS;
        if (!grid[r][c].isObstacle && !grid[r][c].hasTank0 && !grid[r][c].hasTank1) {
            switch (i) {
            case 0:
                grid[r][c].hasTank0 = true;
                tanks[0] = Tank(r, c, 0, 100, BLUE);
                break;
            case 1:
                grid[r][c].hasTank0 = true;
                tanks[1] = Tank(r, c, 0, 100, RED);
                break;
            case 2:
                grid[r][c].hasTank1 = true;
                tanks[2] = Tank(r, c, 1, 100, SKYBLUE);
                break;
            case 3:
                grid[r][c].hasTank1 = true;
                tanks[3] = Tank(r, c, 1, 100, YELLOW);
                break;
            }
            i++;
        }
    }
    buildAdjacency();
}

void Map::buildAdjacency() {
    int total = ROWS * COLS;
    for (int i = 0; i < total; i++)
        for (int j = 0; j < total; j++)
            adjacency[i][j] = false;

    int dr[] = { -1, 1, 0, 0 };
    int dc[] = { 0, 0, -1, 1 };

    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (grid[r][c].isObstacle) continue;
            for (int d = 0; d < 4; d++) {
                int nr = r + dr[d];
                int nc = c + dc[d];
                if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS)
                    if (!grid[nr][nc].isObstacle)
                        adjacency[toIndex(r, c)][toIndex(nr, nc)] = true;
            }
        }
    }
}

bool Map::isFullyConnected() const {
    int total = ROWS * COLS;
    int start = -1;
    for (int i = 0; i < total; i++) {
        int r = i / COLS, c = i % COLS;
        if (!grid[r][c].isObstacle) { start = i; break; }
    }
    if (start == -1) return false;

    bool* visited = new bool[total];
    for (int i = 0; i < total; i++) visited[i] = false;

    int* q = new int[total];
    int front = 0, back = 0;
    q[back++] = start;
    visited[start] = true;

    while (front < back) {
        int curr = q[front++];
        for (int j = 0; j < total; j++) {
            if (adjacency[curr][j] && !visited[j]) {
                visited[j] = true;
                q[back++] = j;
            }
        }
    }

    bool connected = true;
    for (int i = 0; i < total; i++) {
        int r = i / COLS, c = i % COLS;
        if (!grid[r][c].isObstacle && !visited[i]) {
            connected = false;
            break;
        }
    }

    delete[] visited;
    delete[] q;
    return connected;
}

void Map::draw() {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (grid[r][c].hasTank0 || grid[r][c].hasTank1) {
                for (const Tank& tank : tanks) {
                    if (tank.row == r && tank.col == c) {
                        DrawRectangle(c * CELL_SIZE, r * CELL_SIZE, CELL_SIZE - 1, CELL_SIZE - 1, tank.color);
                        break;
                    }
                }
			}
            else {
                Color color = grid[r][c].isObstacle ? DARKGRAY : (isWhite ? BLACK : WHITE);
                DrawRectangle(c * CELL_SIZE, r * CELL_SIZE, CELL_SIZE - 1, CELL_SIZE - 1, color);
            }
        }
    }
    if (highlightCol >= 0 && highlightRow >= 0) {
		Rectangle rect = { (float)(highlightCol * CELL_SIZE), (float)(highlightRow * CELL_SIZE), (float)(CELL_SIZE - 1), (float)(CELL_SIZE - 1) };
        DrawRectangleLinesEx(rect, 3, GREEN);
	}
}

void Map::updateMovement() {
    static float timer = 0;
    timer += GetFrameTime();
    if (!moving || timer < 0.1f) return;
    timer = 0;

    if (path.size() > 1) {
        Cell* current = path[0];
        Cell* next = path[1];

        if (selectedTank < 2) {
            (*current).hasTank0 = false;
            (*next).hasTank0 = true;
        }
        else {
            (*current).hasTank1 = false;
            (*next).hasTank1 = true;
        }
        tanks[selectedTank].row = (*next).row;
        tanks[selectedTank].col = (*next).col;
        path.erase(path.begin());
    }
    else {
        moving = false;
        path.clear();
        selectedTank = -1;
    }
}

bool Map::change() {
    return false;
}