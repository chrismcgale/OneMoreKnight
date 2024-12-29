// game.c
#include <raylib.h>
#include <raymath.h>
#include "game.h"
#include "weapon.h"
#include <math.h>
#include <stdio.h>



GameState* Game_Create(void) {
    GameState* game = (GameState*)malloc(sizeof(GameState));
    
    // Create player at center of screen
    game->player = Player_Create((Vector2){
        SCREEN_WIDTH / 2.0f,
        SCREEN_HEIGHT / 2.0f
    });

    // Initialize game state
    game->currentNight = 1;
    game->vampireCount = 0;
    game->gameOver = false;
    game->BulletCount = 0;

    // Initialize arrays
    for (int i = 0; i < MAX_VAMPIRES; i++) {
        game->vampires[i] = NULL;
    }
    
    for (int i = 0; i < MAX_BULLETS; i++) {
        game->bullets[i].active = false;
    }
    
    for (int i = 0; i < MAX_LOOT; i++) {
        game->loot[i].active = false;
    }

    // Spawn initial vampires
    for (int i = 0; i < 5; i++) {
        Game_SpawnVampire(game);
    }

    return game;
}

void Game_Destroy(GameState* game) {
    // Clean up player
    Player_Destroy(game->player);
    
    // Clean up vampires
    for (int i = 0; i < MAX_VAMPIRES; i++) {
        if (game->vampires[i] != NULL) {
            Vampire_Destroy(game->vampires[i]);
        }
    }
    
    // Clean up game state
    free(game);
}

void Game_SpawnVampire(GameState* game) {
    if (game->vampireCount >= MAX_VAMPIRES) return;

    // Find empty slot
    for (int i = 0; i < MAX_VAMPIRES; i++) {
        if (game->vampires[i] == NULL) {
            // Spawn position logic
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

            game->vampires[i] = Vampire_Create(pos, game->currentNight);
            game->vampireCount++;
            break;
        }
    }
}



void Game_SpawnLoot(GameState* game, Vector2 position) {
    if (GetRandomValue(0, 100) / 100.0f > LOOT_CHANCE) return;

    for (int i = 0; i < MAX_LOOT; i++) {
        if (!game->loot[i].active) {
            game->loot[i].position = position;
            game->loot[i].active = true;
            game->loot[i].type = GetRandomValue(0, 2);

            if (game->loot[i].type == LOOT_WEAPON) {
                game->loot[i].data.weapon = GetRandomValue(0, 2);
            } else if (game->loot[i].type == LOOT_TALISMAN) {
                game->loot[i].data.talisman = GetRandomValue(0, 2);
            }
            break;
        }
    }
}

void Game_HandleLootCollection(GameState* game) {
    Vector2 playerPos = Player_GetPosition(game->player);
    float playerRadius = Player_GetRadius(game->player);

    for (int i = 0; i < MAX_LOOT; i++) {
        if (game->loot[i].active) {
            if (CheckCollisionCircles(
                playerPos, playerRadius,
                game->loot[i].position, 10.0f)) {
                
                switch (game->loot[i].type) {
                    case LOOT_LIFE:
                        // Add life handled internally by player
                        Player_AddLife(game->player);
                        break;
                    case LOOT_WEAPON:
                        game->BulletCount = 0;
                        Player_AddWeapon(game->player, Weapon_Create(game->loot[i].data.weapon));
                        break;
                    case LOOT_TALISMAN:
                        Player_AddTalisman(game->player, game->loot[i].data.talisman);
                        break;
                }
                game->loot[i].active = false;
            }
        }
    }
}

void updateSwordSwing(GameState* game, Player* p, Vector2 playerPos, Vector2 mousePos, Vampire** enemies, int vampCount) {
    if (p->currentWeapon->type == WEAPON_SWORD && p->isSwinging) {
        // Update swing animation
        p->currentAngle += p->currentWeapon->projectileSpeed;
        
        // Check if swing animation is complete
        if (p->currentAngle >= p->startAngle + PI) {
            p->isSwinging = false;
            return;
        }
        
        // Check for enemies in the swing arc
        for (int i = 0; i < vampCount; i++) {
            if (!enemies[i]->active) continue;
            
            // Get vector to enemy
            Vector2 toEnemy = Vector2Subtract(enemies[i]->position, playerPos);
            float distanceToEnemy = Vector2Length(toEnemy);
            
            // Check if enemy is within sword reach
            if (distanceToEnemy <=  p->currentWeapon->projectileRadius) {
                // Get angle to enemy
                float enemyAngle = atan2f(toEnemy.y, toEnemy.x);
                //if (enemyAngle < 0) enemyAngle += 2*PI;  // Normalize angle
                
                // Check if enermy is within current swing arc
                float swingStart = p->startAngle;
                float swingEnd = p->currentAngle;
                
                if (swingStart <= enemyAngle && enemyAngle <= swingEnd) {
                    // Hit enemy
                    Vampire_TakeDamage(enemies[i], p->currentWeapon->damage);
                }
            }
            if (!Vampire_IsActive(game->vampires[i])) {
                Game_SpawnLoot(game, Vampire_GetPosition(game->vampires[i]));
                Vampire_Destroy(game->vampires[i]);
                game->vampires[i] = NULL;
                game->vampireCount--;
            }
        }
    }
}

void Game_Update(GameState* game) {
    if (game->gameOver) {
        if (IsKeyPressed(KEY_R)) {
            // Reset game
            Game_Destroy(game);
            game = Game_Create();
        }
        return;
    }

    // Update player
    Player_Update(game->player);

    updateSwordSwing(game, game->player, Player_GetPosition(game->player), GetMousePosition(), game->vampires, game->vampireCount);

    // Handle shooting
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        handleWeaponInput(Player_GetPosition(game->player), GetMousePosition(), game->bullets, &game->BulletCount, game->player->currentWeapon, &game->player->startAngle, &game->player->currentAngle, &game->player->isSwinging);
        Player_UpdateShootCooldown(game->player);
    }

    // Update bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (game->bullets[i].active) {
            game->bullets[i].position.x += game->bullets[i].velocity.x * 7.0f;
            game->bullets[i].position.y += game->bullets[i].velocity.y * 7.0f;

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
    Vector2 playerPos = Player_GetPosition(game->player);
    float playerRadius = Player_GetRadius(game->player);

    for (int i = 0; i < MAX_VAMPIRES; i++) {
        if (game->vampires[i] != NULL && Vampire_IsActive(game->vampires[i])) {
            // Update vampire
            Vampire_Update(game->vampires[i], playerPos);

            // Check collision with player
            if (CheckCollisionCircles(
                playerPos, playerRadius,
                Vampire_GetPosition(game->vampires[i]), 
                Vampire_GetRadius(game->vampires[i]))) {
                
                Player_TakeDamage(game->player);
                Vampire_Destroy(game->vampires[i]);
                game->vampires[i] = NULL;
                game->vampireCount--;
                
                if (game->player->lives <= 0) {
                    game->gameOver = true;
                }
            }

            // Check collision with bullets
            if (game->vampires[i] != NULL) {  // Check again as vampire might have been destroyed
                for (int j = 0; j < MAX_BULLETS; j++) {
                    if (game->bullets[j].active) {
                        handleProjectileEnemyCollision(&game->bullets[j], game->vampires[i]);

                        if (!Vampire_IsActive(game->vampires[i])) {
                            Game_SpawnLoot(game, Vampire_GetPosition(game->vampires[i]));
                            Vampire_Destroy(game->vampires[i]);
                            game->vampires[i] = NULL;
                            game->vampireCount--;
                            break;
                        }
                    }
                }
            }
        }   
    }

    // Handle loot collection
    Game_HandleLootCollection(game);

    // Spawn new vampires if needed
    if (game->vampireCount < 5 + game->currentNight * 2) {
        Game_SpawnVampire(game);
    }

    // Check for night progression
    if (game->vampireCount == 0) {
        game->currentNight++;
        for (int i = 0; i < 5 + game->currentNight * 2; i++) {
            Game_SpawnVampire(game);
        }
    }
}

void DrawSwordSwing(Player* p, Vector2 playerPos) {
    if (p->isSwinging) {
        // Draw sword blade
        Vector2 swordTip = {
            playerPos.x + p->currentWeapon->projectileRadius * cosf(p->currentAngle),
            playerPos.y + p->currentWeapon->projectileRadius * sinf(p->currentAngle)
        };
        DrawLineEx(playerPos, swordTip, 3.0f, WHITE);
        
        // Draw swing arc visualization (optional, for debugging)
        DrawCircleSector(
            playerPos,
            p->currentWeapon->projectileRadius,
            RAD2DEG * p->startAngle,
            RAD2DEG * p->currentAngle,
            32,
            Fade(WHITE, 0.2f)
        );
    }
}

void Game_Draw(GameState* game) {
    BeginDrawing();
    ClearBackground(DARKGRAY);

    // Draw player
    Player_Draw(game->player);

    // Draw sword swing
    DrawSwordSwing(game->player, Player_GetPosition(game->player));

    // Draw vampires
    for (int i = 0; i < MAX_VAMPIRES; i++) {
        if (game->vampires[i] != NULL && Vampire_IsActive(game->vampires[i])) {
            Vampire_Draw(game->vampires[i]);
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
        TextFormat("Night: %d", game->currentNight),
        SCREEN_WIDTH - 100, 20, 20, BLACK
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