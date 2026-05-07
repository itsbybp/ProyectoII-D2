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
        BeginDrawing();
        ClearBackground(DARKGRAY);
        map.draw();
		map.selectedCol = -1;
		map.selectedRow = -1;

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mouse = GetMousePosition();
            int col = static_cast<int>(mouse.x) / Map::CELL_SIZE;
            int row = static_cast<int>(mouse.y) / Map::CELL_SIZE;

			map.selectedCol = col;
            map.selectedRow = row;

            if (row >= 0 && row < Map::ROWS && col >= 0 && col < Map::COLS) {
                if (map.grid[row][col].hasTank0 && map.player == 0) {
                    int tank;
                    if (map.tanks[0].row == row && map.tanks[0].col == col) tank = 0;
                    else if (map.tanks[1].row == row && map.tanks[1].col == col) tank = 1;
                    else if (map.tanks[2].row == row && map.tanks[2].col == col) tank = 2;
                    else if (map.tanks[3].row == row && map.tanks[3].col == col) tank = 3;
                    bool a = true;
                    while (a) {
                        EndDrawing();
                        BeginDrawing();
                        ClearBackground(DARKGRAY);
                        map.draw();
                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                            Vector2 target = GetMousePosition();
                            int targetCol = static_cast<int>(target.x) / Map::CELL_SIZE;
                            int targetRow = static_cast<int>(target.y) / Map::CELL_SIZE;

                            if (!map.grid[targetRow][targetCol].isObstacle && !map.grid[targetRow][targetCol].hasTank0 && !map.grid[targetRow][targetCol].hasTank1) {
                                if (tank == 0) {
                                    srand((unsigned int)time(nullptr));
                                    int randNum = rand() % 2;
                                    if (randNum == 0) {
                                        map.tanks[0].moveBFS(0, map, map.grid[row][col], map.grid[targetRow][targetCol]);
                                    }
                                    else {
										map.tanks[0].moveRandom(0, map, map.grid[row][col], map.grid[targetRow][targetCol]);
                                    }
                                }
                                else if (tank == 1) {
                                    srand((unsigned int)time(nullptr));
                                    int randNum = rand() % 10;
                                    if (randNum < 7) {
                                        map.tanks[1].moveDijkstra(0, map, map.grid[row][col], map.grid[targetRow][targetCol]);
                                    }
                                    else {
										map.tanks[1].moveRandom(0, map, map.grid[row][col], map.grid[targetRow][targetCol]);
                                    }
                                }
                                else if (tank == 2) {
                                    srand((unsigned int)time(nullptr));
                                    int randNum = rand() % 2;
                                    if (randNum == 0) {
                                        map.tanks[2].moveBFS(0, map, map.grid[row][col], map.grid[targetRow][targetCol]);
                                    }
                                    else {
                                        map.tanks[2].moveRandom(0, map, map.grid[row][col], map.grid[targetRow][targetCol]);
                                    }
                                }
                                else if (tank == 3) {
                                    srand((unsigned int)time(nullptr));
                                    int randNum = rand() % 10;
                                    if (randNum < 7) {
                                        map.tanks[3].moveDijkstra(0, map, map.grid[row][col], map.grid[targetRow][targetCol]);
                                    }
                                    else {
                                        map.tanks[3].moveRandom(0, map, map.grid[row][col], map.grid[targetRow][targetCol]);
                                    }
                                }
							}
                            
                            if (targetRow >= 0 && targetRow < Map::ROWS && targetCol >= 0 && targetCol < Map::COLS) {
                                for (int i = 0; i < 2; i++) {
                                    if (map.tanks[i].row == row && map.tanks[i].col == col && !map.grid[targetRow][targetCol].isObstacle && !map.grid[targetRow][targetCol].hasTank0 && !map.grid[targetRow][targetCol].hasTank1) {
                                        map.tanks[i].row = targetRow;
                                        map.tanks[i].col = targetCol;
                                        map.grid[row][col].hasTank0 = false;
                                        map.grid[row][col].isObstacle = false;
                                        map.grid[targetRow][targetCol].hasTank0 = true;
                                        a = false;
                                        break;
                                    }
                                }
                            }
						}
                    }
                    map.isWhite = false;
                    map.player = 1;
                }

                else if (map.grid[row][col].hasTank1 && map.player == 1) {
                    bool a = true;
                    while (a) {
                        EndDrawing();
                        BeginDrawing();
                        ClearBackground(DARKGRAY);
                        map.draw();
                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                            Vector2 target = GetMousePosition();
                            int targetCol = static_cast<int>(target.x) / Map::CELL_SIZE;
                            int targetRow = static_cast<int>(target.y) / Map::CELL_SIZE;
                            if (targetRow >= 0 && targetRow < Map::ROWS && targetCol >= 0 && targetCol < Map::COLS) {
                                for (int i = 2; i < 4; i++) {
                                    if (map.tanks[i].row == row && map.tanks[i].col == col && !map.grid[targetRow][targetCol].hasTank0 && !map.grid[targetRow][targetCol].hasTank1) {
                                        map.tanks[i].row = targetRow;
                                        map.tanks[i].col = targetCol;
                                        map.grid[row][col].hasTank1 = false;
										map.grid[row][col].isObstacle = false;
                                        map.grid[targetRow][targetCol].hasTank1 = true;
                                        a = false;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    map.isWhite = true;
                    map.player = 0;
				}
            }
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}