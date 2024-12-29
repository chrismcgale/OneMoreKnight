// weapon.c
#include "weapon.h"
#include <raymath.h>
#include <stdlib.h>
#include <stdio.h>

Weapon* Weapon_Create(WeaponType type) {
    Weapon* self = (Weapon*)malloc(sizeof(Weapon));
    self->type = type;
    switch(type) {
        case WEAPON_PISTOL:
            self->damage = 5.0f;
            self->projectileSpeed = 10.0f;
            self->projectileRadius = 3.0f;
            self->shootCooldown = 0.2f;
            self->maxProjectiles = 20;
            break;
        case WEAPON_SHOTGUN:
            self->damage = 10.0f;
            self->projectileSpeed = 5.0f;
            self->projectileRadius = 5.0f;
            self->shootCooldown = 0.5f;
            self->maxProjectiles = 25;
            break;
        case WEAPON_CROSSBOW:  
            self->damage = 15.0f;
            self->projectileSpeed = 7.0f;
            self->projectileRadius = 4.0f;
            self->shootCooldown = 1.0f;
            self->maxProjectiles = 10;
            break;
        case WEAPON_MINIGUN:
            self->damage = 2.0f;
            self->projectileSpeed = 20.0f;
            self->projectileRadius = 2.0f;
            self->shootCooldown = 0.1f;
            self->maxProjectiles = 50;
            break;
        }
    self->bulletsLeft = self->maxProjectiles;
    return self;
}

void Weapon_Destroy(Weapon* self) {
    free(self);
}

// For pistol fire
void shootPistol(Vector2 playerPos, Vector2 direction, Projectile projectiles[], int* projectileCount, Weapon* currentWeapon) {
    // Find inactive bullet slot
        if (*projectileCount < currentWeapon->maxProjectiles && currentWeapon->bulletsLeft > 0) {
            projectiles[*projectileCount].position = playerPos;
            projectiles[*projectileCount].velocity = Vector2Scale(Vector2Normalize(direction), currentWeapon->projectileSpeed);
            projectiles[*projectileCount].active = true;
            projectiles[*projectileCount].damage = currentWeapon->damage;
            projectiles[*projectileCount].radius = currentWeapon->projectileRadius;
            (*projectileCount)++;
            currentWeapon->bulletsLeft--;
        }
}

// For shotgun spread
void shootShotgun(Vector2 playerPos, Vector2 direction, Projectile projectiles[], int* projectileCount, Weapon* currentWeapon) {
    float spreadAngle = 30.0f;  // Total spread angle in degrees
    int pelletCount = 5;        // Number of pellets
    
    for (int i = 0; i < pelletCount; i++) {
        if (*projectileCount < currentWeapon->maxProjectiles && currentWeapon->bulletsLeft > 0) {
            float angle = -spreadAngle/2.0f + (spreadAngle/(pelletCount-1)) * i;
            float radians = angle * PI / 180.0f;
            
            // Rotate the direction vector by the spread angle
            Vector2 spreadDir = {
                direction.x * cos(radians) - direction.y * sin(radians),
                direction.x * sin(radians) + direction.y * cos(radians)
            };
            
            projectiles[*projectileCount].position = playerPos;
            projectiles[*projectileCount].velocity = Vector2Scale(Vector2Normalize(spreadDir), currentWeapon->projectileSpeed);
            projectiles[*projectileCount].active = true;
            projectiles[*projectileCount].penetrationsLeft = 1;  // Single hit for shotgun pellets
            projectiles[*projectileCount].damage = currentWeapon->damage;
            projectiles[*projectileCount].radius = currentWeapon->projectileRadius;
            (*projectileCount)++;
            currentWeapon->bulletsLeft--;
        }
    }
}

// For crossbow piercing
void shootCrossbow(Vector2 playerPos, Vector2 direction, Projectile projectiles[], int* projectileCount, Weapon* currentWeapon) {
    if (*projectileCount < currentWeapon->maxProjectiles && currentWeapon->bulletsLeft > 0) {
        projectiles[*projectileCount].position = playerPos;
        projectiles[*projectileCount].velocity = Vector2Scale(Vector2Normalize(direction), currentWeapon->projectileSpeed);
        projectiles[*projectileCount].active = true;
        projectiles[*projectileCount].penetrationsLeft = 3;  // Can pierce through 3 enemies
        projectiles[*projectileCount].damage = currentWeapon->damage;
        projectiles[*projectileCount].radius = currentWeapon->projectileRadius;
        (*projectileCount)++;
        currentWeapon->bulletsLeft--;
    }
}

void handleProjectileEnemyCollision(Projectile* projectile, Vampire* vamp) {
    if (CheckCollisionCircles(projectile->position, projectile->radius, 
                            vamp->position, vamp->radius)) {
        // Handle enemy damage/death
        Vampire_TakeDamage(vamp, projectile->damage);
        
        // Handle projectile penetration
        projectile->penetrationsLeft--;
        if (projectile->penetrationsLeft <= 0) {
            projectile->active = false;
        }

        
    }
}

//For weapon switching and usage, you can modify your input handling:
void handleWeaponInput(Vector2 playerPos, Vector2 mousePos, Projectile projectiles[], int* projectileCount, Weapon* currentWeapon) {

    Vector2 direction = Vector2Normalize(Vector2Subtract(mousePos, playerPos));

    switch(currentWeapon->type) {
        case WEAPON_PISTOL:
            shootPistol(playerPos, direction, projectiles, projectileCount, currentWeapon);
            break;
        case WEAPON_SHOTGUN:
            shootShotgun(playerPos, direction, projectiles, projectileCount, currentWeapon);
            break;
        case WEAPON_CROSSBOW:
            shootCrossbow(playerPos, direction, projectiles, projectileCount, currentWeapon);
            break;
        
    }
}
