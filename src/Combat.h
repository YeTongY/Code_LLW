#ifndef COMBAT_H
#define COMBAT_H

#include "raylib.h"
#include "Player.h"
#include "Enemy.h"

// 战斗系统类
class Combat {
private:
    bool isActive;
    int turn;
    
public:
    Combat();
    ~Combat();
    
    void Start(Player* player, Enemy* enemy);
    void Update();
    void Draw();
    void End();
    bool IsActive();
};

#endif // COMBAT_H
