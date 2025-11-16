#pragma once

#include "raylib.h"  // 需要 Texture2D 类型定义

// 前向声明（打破循环依赖）
struct GameContext;

typedef enum {//枚举玩家方向
    PLAYER_DIR_DOWN = 0, // 面向下方
    PLAYER_DIR_UP,       // 面向上方
    PLAYER_DIR_LEFT,     // 面向左方
    PLAYER_DIR_RIGHT     // 面向右方
} PlayerDirection;

struct Stats {//声明玩家基础属性
    int hp;//当前hp
    int maxHp;//hp上限
    int attack;//攻击力
    int defense;//防御力
};



struct Player {//声明玩家主结构体
    int gridX; // 玩家在"网格"上的 X 坐标 (逻辑)
    int gridY; // 玩家在"网格"上的 Y 坐标 (逻辑)
    
    Stats stats; // 玩家的属性
    
    // (P1 阶段)
    Texture2D spriteSheet; // P1: 玩家的精灵图贴图
    PlayerDirection currentDirection; // 玩家"当前"的方向
};



 /**
  * @brief 用于读取键盘输入来更新玩家位置
  * 
  * @param ctx Gamestate里的GameContext核心数据文件
  */
void updatePlayer(GameContext& ctx);

/**
 * @brief 角色摄像机，负责绘制非战斗情况，即大地图时的画面
 * 
 * @param ctx Gamestate里的GameContext核心数据文件
 */
void drawPlayer(const GameContext& ctx);

/**
 * @brief (P1) 加载玩家所需的美术资源 (贴图)
 * (这个函数必须在 main() 的“初始化”中被调用)
 * @param ctx 
 */
void LoadPlayerAssets(GameContext& ctx);

/**
 * @brief (P1) 卸载玩家的美术资源
 * (这个函数必须在 main() 的“清理”中被调用)
 * @param ctx 
 */
void UnloadPlayerAssets(GameContext& ctx);