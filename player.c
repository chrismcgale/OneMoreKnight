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
    self->currentWeapon = Weapon_Create(WEAPON_SHOTGUN);
    self->currentTalisman = TALISMAN_NONE;
    self->lastShootTime = 0;
    self->speed = 5.0f;
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
}

void drawWeaponIndicator(Weapon* currentWeapon) {
    // Assuming health bar is at top of screen, position this below it
    Rectangle iconBackground = {10, 50, 100, 40};  // x, y, width, height
    DrawRectangleRec(iconBackground, DARKGRAY);
    DrawRectangleLinesEx(iconBackground, 2, WHITE);  // Border

    // Draw ammo counter text
    char ammoText[32];
    sprintf(ammoText, "%d/%d", currentWeapon->bulletsLeft, currentWeapon->maxProjectiles);
    DrawText(ammoText, 110, 65, 15, WHITE);

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
    }
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
        DrawCircle(30 + i * 30, 30, 10, RED);
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