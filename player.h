// player.h
#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "talisman.h"
#include "weapon.h"

typedef struct Player {
    Vector2 position;
    float radius;
    int lives;
    Weapon* currentWeapon;
    TalismanType currentTalisman;
    float lastShootTime;
    float speed;
    bool isSwinging;
    float startAngle;
    float currentAngle;
} Player;

// Constructor and destructor
Player* Player_Create(Vector2 position);
void Player_Destroy(Player* self);

// Methods
void Player_Update(Player* self);
void Player_Draw(Player* self);
void Player_AddLife(Player* self);
void Player_TakeDamage(Player* self);
void Player_AddWeapon(Player* self, Weapon* weapon);
void Player_AddTalisman(Player* self, TalismanType talisman);
bool Player_CanShoot(Player* self);
void Player_UpdateShootCooldown(Player* self);
Vector2 Player_GetPosition(Player* self);
float Player_GetRadius(Player* self);

#endif
