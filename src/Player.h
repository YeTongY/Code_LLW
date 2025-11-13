#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

// 玩家角色类
class Player {
private:
    Vector2 position;
    int hp;
    int maxHp;
    int attack;
    int defense;
    float speed;
    
public:
    Player();
    ~Player();
    
    void Update();
    void Draw();
    void Move(Vector2 direction);
    void TakeDamage(int damage);
    void Heal(int amount);
    
    // Getters
    Vector2 GetPosition() const { return position; }
    int GetHP() const { return hp; }
    int GetMaxHP() const { return maxHp; }
};

#endif // PLAYER_H
