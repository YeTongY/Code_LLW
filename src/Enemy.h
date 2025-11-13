#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"

// 敌人角色类
class Enemy {
private:
    Vector2 position;
    int hp;
    int maxHp;
    int attack;
    int defense;
    const char* name;
    
public:
    Enemy(const char* enemyName, Vector2 pos);
    ~Enemy();
    
    void Update();
    void Draw();
    void TakeDamage(int damage);
    int Attack();
    
    // Getters
    Vector2 GetPosition() const { return position; }
    int GetHP() const { return hp; }
    bool IsDead() const { return hp <= 0; }
};

#endif // ENEMY_H
