// vampire.h
#ifndef VAMPIRE_H
#define VAMPIRE_H

#include "raylib.h"

typedef struct Vampire {
    Vector2 position;
    float radius;
    float health;
    float speed;
    bool active;
} Vampire;

// Constructor and destructor
Vampire* Vampire_Create(Vector2 position, int night);
void Vampire_Destroy(Vampire* self);

// Methods
void Vampire_Update(Vampire* self, Vector2 targetPos);
void Vampire_Draw(Vampire* self);
void Vampire_TakeDamage(Vampire* self, float damage);
bool Vampire_IsActive(Vampire* self);
Vector2 Vampire_GetPosition(Vampire* self);
float Vampire_GetRadius(Vampire* self);
float Vampire_GetHealth(Vampire* self);

#endif