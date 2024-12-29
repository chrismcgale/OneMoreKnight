// player.c
#include "player.h"
#include "weapon.h"
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <stdio.h>

Player* Player_Create(Vector2 position) {
    Player* self = (Player*)malloc(sizeof(Player));
    if (self == NULL) {
        return NULL;
    }
    self->position = position;
    self->radius = 20.0f;
    self->lives = 3;
    self->currentWeapon = Weapon_Create(WEAPON_SWORD);
    self->currentTalisman = TALISMAN_NONE;
    self->lastShootTime = 0;
    self->speed = 5.0f;
    self->isSwinging = false;
    return self;
}

void Player_Destroy(Player* self) {
    free(self);
}

void Player_Update(Player* self) {
    Vector2 movement = {0};
    if (IsKeyDown(KEY_W)) movement.y -= 1.0f;
    if (IsKeyDown(KEY_S)) movement.y += 1.0f;
    if (IsKeyDown(KEY_A)) movement.x -= 1.0f;
    if (IsKeyDown(KEY_D)) movement.x += 1.0f;

    if (movement.x != 0 || movement.y != 0) {
        movement = Vector2Normalize(movement);
        float currentSpeed = self->speed;
        if (self->currentTalisman == TALISMAN_SPEED) {
            currentSpeed *= 1.5f;
        }
        
        self->position.x += movement.x * currentSpeed;
        self->position.y += movement.y * currentSpeed;

        // Keep player in bounds
        self->position.x = Clamp(self->position.x, self->radius, 
            GetScreenWidth() - self->radius);
        self->position.y = Clamp(self->position.y, self->radius, 
            GetScreenHeight() - self->radius);
    }

    if (self->currentWeapon->bulletsLeft == 0) {
        Player_AddWeapon(self, Weapon_Create(WEAPON_SWORD));
    }
}


void drawWeaponIndicator(Weapon* currentWeapon) {
    // Assuming health bar is at top of screen, position this below it
    Rectangle iconBackground = {10, 50, 150, 40};  // x, y, width, height
    DrawRectangleRec(iconBackground, DARKGRAY);
    DrawRectangleLinesEx(iconBackground, 2, WHITE);  // Border

    if (currentWeapon->type != WEAPON_SWORD) {
        // Draw ammo counter text
        char ammoText[32];
        DrawText(ammoText, 70, 70, 15, WHITE);
    }

    // Draw weapon icon based on current weapon
    switch(currentWeapon->type) {
        case WEAPON_SHOTGUN:
            // Draw shotgun icon
            DrawRectangle(20, 60, 30, 8, WHITE);    // Barrel
            DrawRectangle(50, 55, 15, 20, WHITE);   // Body
            DrawRectangle(45, 65, 10, 15, WHITE);   // Handle
            DrawText("SHOTGUN", 70, 60, 10, WHITE);
            break;
            
        case WEAPON_CROSSBOW:
            // Draw crossbow icon
            DrawLine(20, 70, 60, 70, WHITE);        // Bow horizontal
            DrawLine(40, 55, 40, 85, WHITE);        // Bow vertical
            DrawTriangle(                           // Arrow
                (Vector2){45, 70},
                (Vector2){60, 65},
                (Vector2){60, 75},
                WHITE
            );
            DrawText("CROSSBOW", 70, 60, 10, WHITE);
            break;
        case WEAPON_PISTOL:
            // Draw pistol icon
            DrawRectangle(20, 60, 20, 8, WHITE);     // Barrel
            DrawRectangle(30, 60, 15, 20, WHITE);    // Body/slide
            DrawRectangle(35, 70, 10, 15, WHITE);    // Handle/grip
            DrawText("PISTOL", 70, 60, 10, WHITE);
            break;
            
        case WEAPON_MINIGUN:
            // Draw minigun icon
            DrawRectangle(20, 60, 35, 4, WHITE);     // Top barrel
            DrawRectangle(20, 65, 35, 4, WHITE);     // Middle barrel
            DrawRectangle(20, 70, 35, 4, WHITE);     // Bottom barrel
            DrawRectangle(55, 60, 15, 15, WHITE);    // Body
            DrawRectangle(50, 70, 10, 15, WHITE);    // Handle
            DrawText("MINIGUN", 70, 60, 10, WHITE);
            break;
        case WEAPON_SWORD:
            // Draw sword icon
            DrawRectangle(35, 57, 6, 15, LIGHTGRAY);    // Handle
            DrawRectangle(33, 72, 10, 4, GRAY);         // Guard/Cross-guard
            DrawRectangle(37, 58, 2, 14, GRAY);         // Handle detail
            DrawTriangle(                               // Blade
                (Vector2){38, 57},
                (Vector2){28, 77},
                (Vector2){48, 77},
                WHITE
            );
            DrawText("SWORD", 70, 60, 10, WHITE);
            break;
    }
}

void DrawHeart(int x, int y, int size, Color color) {
    // Draw two circles for the top of the heart
    DrawCircle(x - size/4, y, size/2, color);
    DrawCircle(x + size/4, y, size/2, color);
    
    // Draw triangle for bottom of heart
    Vector2 v1 = { x - size/2, y + size/4 };
    Vector2 v2 = { x + size/2, y + size/4 };
    Vector2 v3 = { x, y + size };
    DrawTriangle(v1, v2, v3, color);
}

void Player_Draw(Player* self) {
    // Base shape - helmet bowl
    DrawCircle(self->position.x, self->position.y - 5, 15, WHITE);  // Head/helmet dome
    // Face plate
    DrawRectangle(self->position.x - 12, self->position.y - 5, 24, 20, WHITE);  // Face guard
    // Cross emblem
    DrawRectangle(self->position.x - 2, self->position.y - 15, 4, 12, RED);     // Vertical cross line
    DrawRectangle(self->position.x - 6, self->position.y - 11, 12, 4, RED);     // Horizontal cross line
    
    // Draw lives
    for (int i = 0; i < self->lives; i++) {
        DrawHeart(30 + (50 * i), 30, 30, RED);
        //DrawCircle(30 + i * 30, 30, 10, RED);
    }

    drawWeaponIndicator(self->currentWeapon);
}


void Player_AddLife(Player* self) {
    self->lives++;
}

void Player_TakeDamage(Player* self) {
    if (self->currentTalisman == TALISMAN_SHIELD) {
        // 50% chance to block damage with shield
        if (GetRandomValue(0, 1) == 1) {
            return;
        }
    }
    self->lives--;
}

void Player_AddWeapon(Player* self, Weapon* weapon) {
    Weapon_Destroy(self->currentWeapon);
    self->currentWeapon = weapon;
}

void Player_AddTalisman(Player* self, TalismanType talisman) {
    self->currentTalisman = talisman;
}

bool Player_CanShoot(Player* self) {
    return GetTime() - self->lastShootTime >= self->currentWeapon->shootCooldown;
}

void Player_UpdateShootCooldown(Player* self) {
    self->lastShootTime = GetTime();
}

Vector2 Player_GetPosition(Player* self) {
    return self->position;
}

float Player_GetRadius(Player* self) {
    return self->radius;
}