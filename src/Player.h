#pragma once

#include "raylib.h"  // 需要 Texture2D 类型定义
#include <vector>
#include <algorithm>

using namespace std;

// 前向声明（打破循环依赖）
struct GameContext;


// 定义实体类型
typedef enum  {
    ENTITY_PLAYER,
    ENTITY_ENEMY,
    ENTITY_NPC,
    ENTITY_PROP // 比如树木、柱子等
}EntityType;

// 渲染命令结构体
struct RenderEntity {
    EntityType type;    // 类型：决定调用哪个绘制函数
    void* data;         // 指针：指向原始数据（如 Player* 或 Enemy*）
    float sortY;        // 排序依据：通常是物体脚底的 Y 坐标
};


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
    float moveSpeed;// 移动速度 像素/秒
    bool isMoving;//玩家移动状态
    Vector2 visualPosition;//玩家渲染位置
    Vector2 moveTarget;//玩家目标移动位置
    vector<int> inputStack;//引入输入栈，以实现后覆盖

    Stats stats; // 玩家的属性
    

    Texture2D spriteSheet; // 玩家的精灵图贴图
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
// GDD v3.13 架构：职责分离
void DrawPlayerSprite(const Player& player);  // 纯粹的精灵绘制
void DrawMapScene(const GameContext& ctx);    // 场景渲染总指挥

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