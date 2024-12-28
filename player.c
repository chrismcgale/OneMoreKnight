// player.c
#include "player.h"
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>

Player* Player_Create(Vector2 position) {
    Player* self = (Player*)malloc(sizeof(Player));
    self->position = position;
    self->radius = 20.0f;
    self->lives = 3;
    self->currentWeapon = WEAPON_PISTOL;
    self->currentTalisman = TALISMAN_NONE;
    self->shootCooldown = 0.5f;
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

void Player_Draw(Player* self) {
    DrawCircle(self->position.x, self->position.y, self->radius, BLUE);
    
    // Draw lives
    for (int i = 0; i < self->lives; i++) {
        DrawCircle(30 + i * 30, 30, 10, RED);
    }
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

void Player_AddWeapon(Player* self, WeaponType weapon) {
    self->currentWeapon = weapon;
    switch (weapon) {
        case WEAPON_PISTOL:
            self->shootCooldown = 0.5f;
            break;
        case WEAPON_SHOTGUN:
            self->shootCooldown = 1.0f;
            break;
        case WEAPON_CROSSBOW:
            self->shootCooldown = 0.75f;
            break;
    }
}

void Player_AddTalisman(Player* self, TalismanType talisman) {
    self->currentTalisman = talisman;
}

bool Player_CanShoot(Player* self) {
    return GetTime() - self->lastShootTime >= self->shootCooldown;
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