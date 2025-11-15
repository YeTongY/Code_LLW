#pragma once

// 前向声明（打破循环依赖）
struct GameContext;

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

 /**
  * @brief 用于读取键盘输入来更新玩家位置
  * 
  * @param ctx Gamestate里的GameContext核心数据文件
  */
void updatePlayer(GameContext& ctx);

void drawPlayer(const GameContext& ctx);