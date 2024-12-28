// main.c
#include "game.h"
#include <time.h>

int main(void) {
    // Initialize window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Vampire Survivor");
    SetTargetFPS(60);

    // Initialize random seed
    srand(time(NULL));

    // Create game state
    GameState* game = Game_Create();

    // Main game loop
    while (!WindowShouldClose()) {
        Game_Update(game);
        Game_Draw(game);
    }

    // Cleanup
    Game_Destroy(game);
    CloseWindow();
    
    return 0;
}