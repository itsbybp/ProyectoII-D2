#include "raylib.h"
#include "Map.h"
#include <cstdlib>
#include <ctime>

int main() {
    const int MAP_W = Map::COLS * Map::CELL_SIZE;
    const int MAP_H = Map::ROWS * Map::CELL_SIZE;
    const int SCREEN_WIDTH = Map::LEFT_PANEL + MAP_W + Map::RIGHT_PANEL;
    const int SCREEN_HEIGHT = MAP_H + Map::BAR_HEIGHT;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tank Attack!");
    SetTargetFPS(60);
    MaximizeWindow();
    srand((unsigned int)time(nullptr));

    Map map;
    map.generate();

    while (!WindowShouldClose()) {
        map.updateMovement();

        // Shift: usar power-up
        if (IsKeyPressed(KEY_LEFT_SHIFT) || IsKeyPressed(KEY_RIGHT_SHIFT)) {
            if (map.isSelected && map.selectedTank != -1 && !map.moving)
                map.applyPowerUp(map.selectedTank);
        }

        // Clicks en panel de botones derecho
        if (map.isSelected && map.selectedTank != -1 && !map.moving) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 mouse = GetMousePosition();
                int panelX = Map::LEFT_PANEL + MAP_W;
                int btnW = Map::RIGHT_PANEL - 20, btnH = 40;
                int btnX = panelX + 10;

                for (int i = 0; i < 3; i++) {
                    int btnY = 40 + i * (btnH + 10);
                    if (mouse.x >= btnX && mouse.x <= btnX + btnW &&
                        mouse.y >= btnY && mouse.y <= btnY + btnH)
                        map.actionMode = i + 1;
                }
            }
        }

        BeginDrawing();
        ClearBackground(Color{ 40,40,40,255 });
        map.draw();

        if (!map.moving) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 mouse = GetMousePosition();
                // Compensar offset del mapa
                int mx = (int)mouse.x - Map::LEFT_PANEL;
                int my = (int)mouse.y;
                int col = mx / Map::CELL_SIZE;
                int row = my / Map::CELL_SIZE;

                if (mx >= 0 && row >= 0 && row < Map::ROWS && col >= 0 && col < Map::COLS) {

                    if (!map.isSelected) {
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

                            Tank& t = map.tanks[map.selectedTank];
                            int u = (map.selectedTank < 2) ? 0 : 1;

                            int probBFS = (map.selectedTank == 0 || map.selectedTank == 2) ? 50 : 0;
                            int probDij = (map.selectedTank == 1 || map.selectedTank == 3) ? 80 : 0;

                            if (t.precisionMoveAct) {
                                probBFS = (map.selectedTank == 0 || map.selectedTank == 2) ? 90 : 0;
                                probDij = (map.selectedTank == 1 || map.selectedTank == 3) ? 90 : 0;
                                t.precisionMoveAct = false;
                                map.usingPowerUp[map.selectedTank] = -1;
                            }

                            if (map.selectedTank == 0 || map.selectedTank == 2) {
                                if (rand() % 100 < probBFS)
                                    t.moveBFS(u, map, map.grid[tr][tc], map.grid[row][col]);
                                else
                                    t.moveRandom(u, map, map.grid[tr][tc], map.grid[row][col]);
                            }
                            else {
                                if (rand() % 100 < probDij)
                                    t.moveDijkstra(u, map, map.grid[tr][tc], map.grid[row][col]);
                                else
                                    t.moveRandom(u, map, map.grid[tr][tc], map.grid[row][col]);
                            }

                            map.endTurn();

                        }
                        else if ((map.player == 0 && map.grid[row][col].hasTank0) ||
                            (map.player == 1 && map.grid[row][col].hasTank1)) {
                            map.highlightRow = row; map.highlightCol = col;
                            if (map.player == 0)
                                map.selectedTank = (map.tanks[0].row == row && map.tanks[0].col == col) ? 0 : 1;
                            else
                                map.selectedTank = (map.tanks[2].row == row && map.tanks[2].col == col) ? 2 : 3;
                            map.actionMode = 0;
                        }
                        else {
                            map.isSelected = false;
                            map.highlightRow = -1; map.highlightCol = -1;
                            map.actionMode = 0;
                        }
                    }
                    else if (map.actionMode == 0) {
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
                int mx = (int)mouse.x - Map::LEFT_PANEL;
                int col = mx / Map::CELL_SIZE;
                int row = (int)mouse.y / Map::CELL_SIZE;

                if (mx >= 0 && row >= 0 && row < Map::ROWS && col >= 0 && col < Map::COLS) {
                    int tr = map.tanks[map.selectedTank].row;
                    int tc = map.tanks[map.selectedTank].col;
                    Tank& t = map.tanks[map.selectedTank];

                    // Aplicar flags de power-up al disparo
                    if (t.attackPwrAct) {
                        map.usePowerAttack = true;
                        t.attackPwrAct = false;
                        map.usingPowerUp[map.selectedTank] = -1;
                    }

                    if (t.precisionAtckAct) {
                        map.usePrecisionAttack = true;
                        t.precisionAtckAct = false;
                        map.usingPowerUp[map.selectedTank] = -1;
                        map.shootBulletAStar(tr, tc, row, col, map.selectedTank);
                    }
                    else {
                        map.shootBullet(tr, tc, row, col, map.selectedTank);
                    }

                    map.endTurn();
                }
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}