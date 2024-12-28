
// game.h
#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "vampire.h"
#include "weapon.h"
#include "talisman.h"
#include <stdlib.h>
#include <time.h>

#define MAX_VAMPIRES 100
#define MAX_BULLETS 50
#define MAX_LOOT 20
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define LOOT_CHANCE 0.3f

typedef struct Bullet {
    Vector2 position;
    Vector2 direction;
    float radius;
    bool active;
    float damage;
} Bullet;

typedef enum {
    LOOT_LIFE,
    LOOT_WEAPON,
    LOOT_TALISMAN
} LootType;

typedef struct Loot {
    Vector2 position;
    LootType type;
    bool active;
    union {
        WeaponType weapon;
        TalismanType talisman;
    } data;
} Loot;

typedef struct GameState {
    Player* player;
    Vampire* vampires[MAX_VAMPIRES];
    Bullet bullets[MAX_BULLETS];
    Loot loot[MAX_LOOT];
    int currentNight;
    int vampireCount;
    bool gameOver;
} GameState;

// Function declarations
GameState* Game_Create(void);
void Game_Destroy(GameState* game);
void Game_Update(GameState* game);
void Game_Draw(GameState* game);
void Game_SpawnVampire(GameState* game);
void Game_ShootBullet(GameState* game);
void Game_SpawnLoot(GameState* game, Vector2 position);
void Game_HandleLootCollection(GameState* game);

#endif