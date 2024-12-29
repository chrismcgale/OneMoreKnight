// weapon.h
#ifndef WEAPON_H
#define WEAPON_H

#include "vampire.h"
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>

typedef enum {
    WEAPON_PISTOL,
    WEAPON_SHOTGUN,
    WEAPON_CROSSBOW,
    WEAPON_MINIGUN
} WeaponType;

typedef struct Weapon {
    WeaponType type;
    float damage;
    float projectileSpeed;
    float projectileRadius;
    float shootCooldown;
    int maxProjectiles;
    int bulletsLeft;
} Weapon;

typedef struct Projectile {
    Vector2 position;
    Vector2 velocity;
    bool active;
    float radius;
    int penetrationsLeft;  // For crossbow piercing
    float damage;
} Projectile;

// Constructor and destructor
Weapon* Weapon_Create(WeaponType type);
void Weapon_Destroy(Weapon* self);

// Methods
void shootShotgun(Vector2 playerPos, Vector2 direction, Projectile projectiles[], int* projectileCount, Weapon* currentWeapon);
void shootCrossbow(Vector2 playerPos, Vector2 direction, Projectile projectiles[], int* projectileCount, Weapon* currentWeapon);
void handleProjectileEnemyCollision(Projectile* projectile, Vampire* vamp);
void handleWeaponInput(Vector2 playerPos, Vector2 mousePos, Projectile projectiles[], int* projectileCount, Weapon* currentWeapon);

#endif