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
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            map.isWhite = !map.isWhite;
		}
        EndDrawing();
    }
    CloseWindow();
    return 0;
}