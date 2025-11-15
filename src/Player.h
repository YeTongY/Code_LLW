#pragma once

struct Stats {//声明玩家基础属性
    int hp;//当前hp
    int maxHp;//hp上限
    int attack;//攻击力
    int defense;//防御力
};

struct Player {//声明玩家主结构体
    int gridX; // 玩家在“网格”上的 X 坐标 (逻辑)
    int gridY; // 玩家在“网格”上的 Y 坐标 (逻辑)
    
    Stats stats; // 玩家的属性
    
    // (P1 阶段)
    // Texture2D playerTexture; // P1: 玩家的贴图
};