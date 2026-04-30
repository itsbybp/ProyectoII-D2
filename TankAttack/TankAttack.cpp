#include "raylib.h"
#include "Map.h"

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
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mouse = GetMousePosition();
            int col = static_cast<int>(mouse.x) / Map::CELL_SIZE;
            int row = static_cast<int>(mouse.y) / Map::CELL_SIZE;
            if (row >= 0 && row < Map::ROWS && col >= 0 && col < Map::COLS) {
                if (map.grid[row][col].hasTank0 && map.player == 0) {
                    map.isWhite = false;
                    map.player = 1;
                }
                else if (map.grid[row][col].hasTank1 && map.player == 1) {
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