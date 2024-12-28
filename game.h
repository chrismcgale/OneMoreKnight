// game.h
#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include <raymath.h>
#include <stdlib.h>
#include <time.h>

#define MAX_VAMPIRES 100
#define MAX_BULLETS 50
#define MAX_LOOT 20
#define PLAYER_SPEED 5.0f
#define VAMPIRE_SPEED 3.0f
#define BULLET_SPEED 7.0f
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define LOOT_CHANCE 0.3f  // 30% chance of dropping loot

typedef enum {
    WEAPON_PISTOL,
    WEAPON_SHOTGUN,
    WEAPON_CROSSBOW
} WeaponType;

typedef enum {
    TALISMAN_SPEED,
    TALISMAN_SHIELD,
    TALISMAN_RAPID_FIRE
} TalismanType;

typedef enum {
    LOOT_LIFE,
    LOOT_WEAPON,
    LOOT_TALISMAN
} LootType;

typedef struct {
    Vector2 position;
    float radius;
    int lives;
    WeaponType weapon;
    TalismanType talisman;
    float shootCooldown;
    float lastShootTime;
} Player;

typedef struct {
    Vector2 position;
    float radius;
    bool active;
    float health;
} Vampire;

typedef struct {
    Vector2 position;
    Vector2 direction;
    float radius;
    bool active;
} Bullet;

typedef struct {
    Vector2 position;
    LootType type;
    bool active;
    union {
        WeaponType weapon;
        TalismanType talisman;
    } data;
} Loot;

typedef struct {
    Player player;
    Vampire vampires[MAX_VAMPIRES];
    Bullet bullets[MAX_BULLETS];
    Loot loot[MAX_LOOT];
    int currentNight;
    int vampireCount;
    bool gameOver;
} GameState;

// Function declarations
void InitGame(GameState *game);
void UpdateGame(GameState *game);
void DrawGame(GameState *game);
void SpawnVampire(GameState *game);
void ShootBullet(GameState *game);
void SpawnLoot(GameState *game, Vector2 position);
void HandleLootCollection(GameState *game);

#endif