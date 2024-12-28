// vampire.c
#include "vampire.h"
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>

Vampire* Vampire_Create(Vector2 position, int night) {
    Vampire* self = (Vampire*)malloc(sizeof(Vampire));
    self->position = position;
    self->radius = 15.0f;
    self->health = night * 1.5f;
    self->speed = 3.0f;
    self->active = true;
    return self;
}

void Vampire_Destroy(Vampire* self) {
    free(self);
}

void Vampire_Update(Vampire* self, Vector2 targetPos) {
    if (!self->active) return;

    Vector2 direction = Vector2Subtract(targetPos, self->position);
    direction = Vector2Normalize(direction);
    self->position = Vector2Add(self->position, 
        Vector2Scale(direction, self->speed));
}

void Vampire_Draw(Vampire* self) {
    if (!self->active) return;
    
    DrawCircle(self->position.x, self->position.y, self->radius, RED);
    
    // Draw health bar
    float healthBarWidth = 30.0f;
    float healthPercentage = self->health / (self->health * 2);
    DrawRectangle(
        self->position.x - healthBarWidth/2,
        self->position.y - self->radius - 10,
        healthBarWidth * healthPercentage,
        5,
        GREEN
    );
}

void Vampire_TakeDamage(Vampire* self, float damage) {
    self->health -= damage;
    if (self->health <= 0) {
        self->active = false;
    }
}

bool Vampire_IsActive(Vampire* self) {
    return self->active;
}

Vector2 Vampire_GetPosition(Vampire* self) {
    return self->position;
}

float Vampire_GetRadius(Vampire* self) {
    return self->radius;
}

float Vampire_GetHealth(Vampire* self) {
    return self->health;
}