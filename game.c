// game.c
#include "game.h"

void InitGame(GameState *game) {
    // Initialize player
    game->player.position = (Vector2){
        SCREEN_WIDTH / 2.0f,
        SCREEN_HEIGHT / 2.0f
    };
    game->player.radius = 20.0f;
    game->player.lives = 3;
    game->player.weapon = WEAPON_PISTOL;
    game->player.talisman = TALISMAN_SPEED;
    game->player.shootCooldown = 0.5f;
    game->player.lastShootTime = 0;

    // Initialize game state
    game->currentNight = 1;
    game->vampireCount = 0;
    game->gameOver = false;

    // Clear arrays
    for (int i = 0; i < MAX_VAMPIRES; i++) {
        game->vampires[i].active = false;
    }
    for (int i = 0; i < MAX_BULLETS; i++) {
        game->bullets[i].active = false;
    }
    for (int i = 0; i < MAX_LOOT; i++) {
        game->loot[i].active = false;
    }

    // Spawn initial vampires
    for (int i = 0; i < 5; i++) {
        SpawnVampire(game);
    }
}

void SpawnVampire(GameState *game) {
    if (game->vampireCount >= MAX_VAMPIRES) return;

    // Find inactive vampire slot
    for (int i = 0; i < MAX_VAMPIRES; i++) {
        if (!game->vampires[i].active) {
            // Spawn vampire at random edge of screen
            float side = (float)GetRandomValue(0, 3);
            Vector2 pos;
            
            if (side == 0) { // Top
                pos.x = GetRandomValue(0, SCREEN_WIDTH);
                pos.y = -50;
            } else if (side == 1) { // Right
                pos.x = SCREEN_WIDTH + 50;
                pos.y = GetRandomValue(0, SCREEN_HEIGHT);
            } else if (side == 2) { // Bottom
                pos.x = GetRandomValue(0, SCREEN_WIDTH);
                pos.y = SCREEN_HEIGHT + 50;
            } else { // Left
                pos.x = -50;
                pos.y = GetRandomValue(0, SCREEN_HEIGHT);
            }

            game->vampires[i].position = pos;
            game->vampires[i].radius = 15.0f;
            game->vampires[i].active = true;
            game->vampires[i].health = game->currentNight * 1.5f;
            game->vampireCount++;
            break;
        }
    }
}

void ShootBullet(GameState *game) {
    float currentTime = GetTime();
    if (currentTime - game->player.lastShootTime < game->player.shootCooldown) {
        return;
    }

    // Find inactive bullet slot
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!game->bullets[i].active) {
            game->bullets[i].position = game->player.position;
            Vector2 mousePos = GetMousePosition();
            Vector2 direction = Vector2Subtract(mousePos, game->player.position);
            game->bullets[i].direction = Vector2Normalize(direction);
            game->bullets[i].radius = 5.0f;
            game->bullets[i].active = true;
            game->player.lastShootTime = currentTime;
            break;
        }
    }
}

void SpawnLoot(GameState *game, Vector2 position) {
    if (GetRandomValue(0, 100) / 100.0f > LOOT_CHANCE) return;

    for (int i = 0; i < MAX_LOOT; i++) {
        if (!game->loot[i].active) {
            game->loot[i].position = position;
            game->loot[i].active = true;
            game->loot[i].type = GetRandomValue(0, 2); // Random loot type

            if (game->loot[i].type == LOOT_WEAPON) {
                game->loot[i].data.weapon = GetRandomValue(0, 2);
            } else if (game->loot[i].type == LOOT_TALISMAN) {
                game->loot[i].data.talisman = GetRandomValue(0, 2);
            }
            break;
        }
    }
}

void HandleLootCollection(GameState *game) {
    for (int i = 0; i < MAX_LOOT; i++) {
        if (game->loot[i].active) {
            if (CheckCollisionCircles(
                game->player.position, game->player.radius,
                game->loot[i].position, 10.0f)) {
                
                switch (game->loot[i].type) {
                    case LOOT_LIFE:
                        game->player.lives++;
                        break;
                    case LOOT_WEAPON:
                        game->player.weapon = game->loot[i].data.weapon;
                        break;
                    case LOOT_TALISMAN:
                        game->player.talisman = game->loot[i].data.talisman;
                        break;
                }
                game->loot[i].active = false;
            }
        }
    }
}

void UpdateGame(GameState *game) {
    if (game->gameOver) {
        if (IsKeyPressed(KEY_R)) {
            InitGame(game);
        }
        return;
    }

    // Update player position
    Vector2 movement = {0};
    if (IsKeyDown(KEY_W)) movement.y -= 1.0f;
    if (IsKeyDown(KEY_S)) movement.y += 1.0f;
    if (IsKeyDown(KEY_A)) movement.x -= 1.0f;
    if (IsKeyDown(KEY_D)) movement.x += 1.0f;

    if (movement.x != 0 || movement.y != 0) {
        movement = Vector2Normalize(movement);
        float speed = PLAYER_SPEED;
        if (game->player.talisman == TALISMAN_SPEED) speed *= 1.5f;
        
        game->player.position.x += movement.x * speed;
        game->player.position.y += movement.y * speed;

        // Keep player in bounds
        game->player.position.x = Clamp(game->player.position.x, 
            game->player.radius, SCREEN_WIDTH - game->player.radius);
        game->player.position.y = Clamp(game->player.position.y, 
            game->player.radius, SCREEN_HEIGHT - game->player.radius);
    }

    // Shooting
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        ShootBullet(game);
    }

    // Update bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (game->bullets[i].active) {
            game->bullets[i].position.x += game->bullets[i].direction.x * BULLET_SPEED;
            game->bullets[i].position.y += game->bullets[i].direction.y * BULLET_SPEED;

            // Check if bullet is out of bounds
            if (game->bullets[i].position.x < 0 || 
                game->bullets[i].position.x > SCREEN_WIDTH ||
                game->bullets[i].position.y < 0 || 
                game->bullets[i].position.y > SCREEN_HEIGHT) {
                game->bullets[i].active = false;
            }
        }
    }

    // Update vampires and check collisions
    for (int i = 0; i < MAX_VAMPIRES; i++) {
        if (game->vampires[i].active) {
            // Move vampire towards player
            Vector2 direction = Vector2Subtract(
                game->player.position, 
                game->vampires[i].position
            );
            direction = Vector2Normalize(direction);
            game->vampires[i].position = Vector2Add(
                game->vampires[i].position,
                Vector2Scale(direction, VAMPIRE_SPEED)
            );

            // Check collision with player
            if (CheckCollisionCircles(
                game->player.position, game->player.radius,
                game->vampires[i].position, game->vampires[i].radius)) {
                game->player.lives--;
                game->vampires[i].active = false;
                game->vampireCount--;
                
                if (game->player.lives <= 0) {
                    game->gameOver = true;
                }
            }

            // Check collision with bullets
            for (int j = 0; j < MAX_BULLETS; j++) {
                if (game->bullets[j].active) {
                    if (CheckCollisionCircles(
                        game->bullets[j].position, game->bullets[j].radius,
                        game->vampires[i].position, game->vampires[i].radius)) {
                        
                        game->vampires[i].health -= 1.0f;
                        game->bullets[j].active = false;

                        if (game->vampires[i].health <= 0) {
                            SpawnLoot(game, game->vampires[i].position);
                            game->vampires[i].active = false;
                            game->vampireCount--;
                        }
                    }
                }
            }
        }
    }

    // Handle loot collection
    HandleLootCollection(game);

    // Spawn new vampires if needed
    if (game->vampireCount < 5 + game->currentNight * 2) {
        SpawnVampire(game);
    }

    // Check for night progression
    if (game->vampireCount == 0) {
        game->currentNight++;
        for (int i = 0; i < 5 + game->currentNight * 2; i++) {
            SpawnVampire(game);
        }
    }
}

void DrawGame(GameState *game) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Draw player
    DrawCircle(
        game->player.position.x,
        game->player.position.y,
        game->player.radius,
        BLUE
    );

    // Draw vampires
    for (int i = 0; i < MAX_VAMPIRES; i++) {
        if (game->vampires[i].active) {
            DrawCircle(
                game->vampires[i].position.x,
                game->vampires[i].position.y,
                game->vampires[i].radius,
                RED
            );
        }
    }

    // Draw bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (game->bullets[i].active) {
            DrawCircle(
                game->bullets[i].position.x,
                game->bullets[i].position.y,
                game->bullets[i].radius,
                BLACK
            );
        }
    }

    // Draw loot
    for (int i = 0; i < MAX_LOOT; i++) {
        if (game->loot[i].active) {
            Color lootColor;
            switch (game->loot[i].type) {
                case LOOT_LIFE: lootColor = GREEN; break;
                case LOOT_WEAPON: lootColor = ORANGE; break;
                case LOOT_TALISMAN: lootColor = PURPLE; break;
            }
            DrawCircle(
                game->loot[i].position.x,
                game->loot[i].position.y,
                10.0f,
                lootColor
            );
        }
    }

    // Draw UI
    DrawText(
        TextFormat("Lives: %d", game->player.lives),
        10, 10, 20, BLACK
    );
    DrawText(
        TextFormat("Night: %d", game->currentNight),
        10, 40, 20, BLACK
    );

    // Draw game over screen
    if (game->gameOver) {
        DrawText(
            "GAME OVER",
            SCREEN_WIDTH/2 - MeasureText("GAME OVER", 40)/2,
            SCREEN_HEIGHT/2 - 40,
            40,
            RED
        );
        DrawText(
            "Press R to restart",
            SCREEN_WIDTH/2 - MeasureText("Press R to restart", 20)/2,
            SCREEN_HEIGHT/2 + 20,
            20,
            BLACK
        );
    }

    EndDrawing();
}