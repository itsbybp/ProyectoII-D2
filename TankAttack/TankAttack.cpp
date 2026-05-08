#include "raylib.h"
#include "Map.h"
#include <cstdlib>
#include <ctime>

int main() {
    const int SCREEN_WIDTH = Map::COLS * Map::CELL_SIZE;
    const int SCREEN_HEIGHT = Map::ROWS * Map::CELL_SIZE;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tank Attack!");
    SetTargetFPS(60);
    MaximizeWindow();

    Map map;
    map.generate();

    while (!WindowShouldClose()) {
        map.updateMovement();
        BeginDrawing();
        ClearBackground(DARKGRAY);
        map.draw();
        map.selectedCol = -1;
        map.selectedRow = -1;

        if (!map.isSelected && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mouse = GetMousePosition();
            int col = static_cast<int>(mouse.x) / Map::CELL_SIZE;
            int row = static_cast<int>(mouse.y) / Map::CELL_SIZE;

            map.selectedCol = col;
            map.selectedRow = row;

            if (row >= 0 && row < Map::ROWS && col >= 0 && col < Map::COLS) {
                if (map.grid[row][col].hasTank0 && map.player == 0) {
                    map.highlightCol = col;
                    map.highlightRow = row;
                    if (map.tanks[0].row == row && map.tanks[0].col == col) map.selectedTank = 0;
                    else if (map.tanks[1].row == row && map.tanks[1].col == col) map.selectedTank = 1;
					map.isSelected = true;
                }
                else if (map.grid[row][col].hasTank1 && map.player == 1) {
                    map.highlightCol = col;
                    map.highlightRow = row;
                    if (map.tanks[2].row == row && map.tanks[2].col == col) map.selectedTank = 2;
                    else if (map.tanks[3].row == row && map.tanks[3].col == col) map.selectedTank = 3;
                    map.isSelected = true;
                }
            }
        }
        else if (map.isSelected && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 target = GetMousePosition();
            int targetCol = static_cast<int>(target.x) / Map::CELL_SIZE;
            int targetRow = static_cast<int>(target.y) / Map::CELL_SIZE;

            int col = map.tanks[map.selectedTank].col;
            int row = map.tanks[map.selectedTank].row;

            if (!map.grid[targetRow][targetCol].isObstacle && !map.grid[targetRow][targetCol].hasTank0 && !map.grid[targetRow][targetCol].hasTank1) {
                map.highlightCol = -1;
                map.highlightRow = -1;

                if (map.selectedTank == 0) {
                    int randNum = rand() % 2;
                    if (randNum == 0) map.tanks[0].moveRandom(0, map, map.grid[row][col], map.grid[targetRow][targetCol]);
                    else map.tanks[0].moveRandom(0, map, map.grid[row][col], map.grid[targetRow][targetCol]);
                    map.isWhite = false;
                    map.player = 1;
                }
                else if (map.selectedTank == 1) {
                    int randNum = rand() % 10;
                    if (randNum < 7) map.tanks[1].moveDijkstra(0, map, map.grid[row][col], map.grid[targetRow][targetCol]);
                    else map.tanks[1].moveRandom(0, map, map.grid[row][col], map.grid[targetRow][targetCol]);
                    map.isWhite = false;
                    map.player = 1;
                }
                else if (map.selectedTank == 2) {
                    int randNum = rand() % 2;
                    if (randNum == 0) map.tanks[2].moveRandom(1, map, map.grid[row][col], map.grid[targetRow][targetCol]);
                    else map.tanks[2].moveRandom(1, map, map.grid[row][col], map.grid[targetRow][targetCol]);
                    map.isWhite = true;
                    map.player = 0;
                } 
                else if (map.selectedTank == 3) {
                    int randNum = rand() % 10;
                    if (randNum < 7) map.tanks[3].moveDijkstra(1, map, map.grid[row][col], map.grid[targetRow][targetCol]);
                    else map.tanks[3].moveRandom(1, map, map.grid[row][col], map.grid[targetRow][targetCol]);
                    map.isWhite = true;
                    map.player = 0;
                }
                map.moving = true;
                map.isSelected = false;
            }
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}