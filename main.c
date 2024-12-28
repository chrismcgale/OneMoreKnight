// main.c
#include "game.h"

int main(void) {
    // Initialize window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Vampire Survivor");
    SetTargetFPS(60);

    // Initialize random seed
    srand(time(NULL));

    // Initialize game state
    GameState game;
    InitGame(&game);

    // Main game loop
    while (!WindowShouldClose()) {
        UpdateGame(&game);
        DrawGame(&game);
    }

    // Cleanup
    CloseWindow();
    return 0;
}