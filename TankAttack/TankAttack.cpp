#include "raylib.h"

int main() {
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tank Attack!");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BEIGE);
        DrawText("Tank Attack!", 320, 280, 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}