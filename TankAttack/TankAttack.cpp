#include "raylib.h"
#include "Map.h"
#include <cstdlib>
#include <ctime>

int main() {
    const int SCREEN_WIDTH = Map::COLS * Map::CELL_SIZE;
    const int SCREEN_HEIGHT = Map::ROWS * Map::CELL_SIZE + Map::BAR_HEIGHT;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tank Attack!");
    SetTargetFPS(60);
    MaximizeWindow();
    srand((unsigned int)time(nullptr));

    Map map;
    map.generate();

    while (!WindowShouldClose()) {
        map.updateMovement();

        // Clicks en botones de la barra (fuera del area del mapa)
        if (map.isSelected && map.selectedTank != -1 && !map.moving) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 mouse = GetMousePosition();
                int barY = Map::ROWS * Map::CELL_SIZE;
                int btnW = 120, btnH = 36;
                int startX = Map::COLS * Map::CELL_SIZE - 3 * (btnW + 10) - 10;
                int btnY = barY + 12;

                for (int i = 0; i < 3; i++) {
                    int x = startX + i * (btnW + 10);
                    if (mouse.x >= x && mouse.x <= x + btnW &&
                        mouse.y >= btnY && mouse.y <= btnY + btnH) {
                        map.actionMode = i + 1;
                    }
                }
            }
        }

        BeginDrawing();
        ClearBackground(DARKGRAY);
        map.draw();

        if (!map.moving) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 mouse = GetMousePosition();
                int col = (int)mouse.x / Map::CELL_SIZE;
                int row = (int)mouse.y / Map::CELL_SIZE;

                // Solo procesar clicks dentro del mapa
                if (row >= 0 && row < Map::ROWS && col >= 0 && col < Map::COLS) {

                    if (!map.isSelected) {
                        // Seleccionar tanque propio
                        if (map.grid[row][col].hasTank0 && map.player == 0) {
                            map.highlightRow = row; map.highlightCol = col;
                            map.selectedTank = (map.tanks[0].row == row && map.tanks[0].col == col) ? 0 : 1;
                            map.isSelected = true;
                            map.showBulletPath = false;
                            map.actionMode = 0;
                        }
                        else if (map.grid[row][col].hasTank1 && map.player == 1) {
                            map.highlightRow = row; map.highlightCol = col;
                            map.selectedTank = (map.tanks[2].row == row && map.tanks[2].col == col) ? 2 : 3;
                            map.isSelected = true;
                            map.showBulletPath = false;
                            map.actionMode = 0;
                        }
                    }
                    else if (map.actionMode == 1) {
                        if (!map.grid[row][col].isObstacle &&
                            !map.grid[row][col].hasTank0 &&
                            !map.grid[row][col].hasTank1) {

                            int tr = map.tanks[map.selectedTank].row;
                            int tc = map.tanks[map.selectedTank].col;
                            map.highlightRow = -1; map.highlightCol = -1;

                            map.buildAdjacencyForTank(map.selectedTank);

                            int u = (map.selectedTank < 2) ? 0 : 1;
                            if (map.selectedTank == 0 || map.selectedTank == 2) {
                                if (rand() % 2 == 0)
                                    map.tanks[map.selectedTank].moveBFS(u, map, map.grid[tr][tc], map.grid[row][col]);
                                else
                                    map.tanks[map.selectedTank].moveRandom(u, map, map.grid[tr][tc], map.grid[row][col]);
                            }
                            else {
                                if (rand() % 10 < 8)
                                    map.tanks[map.selectedTank].moveDijkstra(u, map, map.grid[tr][tc], map.grid[row][col]);
                                else
                                    map.tanks[map.selectedTank].moveRandom(u, map, map.grid[tr][tc], map.grid[row][col]);
                            }

                            map.player = (map.player == 0) ? 1 : 0;
                            map.isSelected = false;
                            map.actionMode = 0;
                        }
                        else if ((map.player == 0 && map.grid[row][col].hasTank0) ||
                            (map.player == 1 && map.grid[row][col].hasTank1)) {
                            // Cambiar seleccion al otro tanque propio
                            map.highlightRow = row; map.highlightCol = col;
                            if (map.player == 0)
                                map.selectedTank = (map.tanks[0].row == row && map.tanks[0].col == col) ? 0 : 1;
                            else
                                map.selectedTank = (map.tanks[2].row == row && map.tanks[2].col == col) ? 2 : 3;
                            map.actionMode = 0;
                        }
                        else {
                            // Deseleccionar
                            map.isSelected = false;
                            map.highlightRow = -1; map.highlightCol = -1;
                            map.actionMode = 0;
                        }
                    }
                    else if (map.actionMode == 0) {
                        // Sin modo: reseleccionar tanque propio
                        if ((map.player == 0 && map.grid[row][col].hasTank0) ||
                            (map.player == 1 && map.grid[row][col].hasTank1)) {
                            map.highlightRow = row; map.highlightCol = col;
                            if (map.player == 0)
                                map.selectedTank = (map.tanks[0].row == row && map.tanks[0].col == col) ? 0 : 1;
                            else
                                map.selectedTank = (map.tanks[2].row == row && map.tanks[2].col == col) ? 2 : 3;
                        }
                        else {
                            map.isSelected = false;
                            map.highlightRow = -1; map.highlightCol = -1;
                        }
                    }
                }
            }

            // Click derecho para disparar
            if (map.isSelected && map.actionMode == 2 &&
                IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                Vector2 mouse = GetMousePosition();
                int col = (int)mouse.x / Map::CELL_SIZE;
                int row = (int)mouse.y / Map::CELL_SIZE;

                if (row >= 0 && row < Map::ROWS && col >= 0 && col < Map::COLS) {
                    int tr = map.tanks[map.selectedTank].row;
                    int tc = map.tanks[map.selectedTank].col;
                    map.shootBullet(tr, tc, row, col, map.selectedTank);
                    map.player = (map.player == 0) ? 1 : 0;
                    map.isSelected = false;
                    map.highlightRow = -1; map.highlightCol = -1;
                    map.actionMode = 0;
                }
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}