/*
2025/11/13 Waring开始编写 游戏状态机
2025/11/14 wyt 修改了头文件保护使其正常生效
2025/11/16 Warning被兼容搞红温 遂把状态机代码改为纯C++
*/
#ifndef GAMESTATE_H
#define GAMESTATE_H

//引入标准库
#include <cstddef>     // size_t
#include <vector>
#include <map>
#include <string>

//引入第三方库
#include "raylib.h"
#include "Player.h"
#include "Map.h"
#include "Enemy.h"

using namespace std;

//前向声明
struct GameContext;
struct GameState;

//状态生命周期函数指针（C++ 风格）
using StateEnterFunc = void (*)(GameContext* ctx, void* state_data);   // 初始化
using StateExitFunc = void (*)(GameContext* ctx, void* state_data);    // 退出
using StateUpdateFunc = void (*)(GameContext* ctx, void* state_data);  // 更新
using StateRenderFunc = void (*)(GameContext* ctx, void* state_data);  // 渲染

//状态机结构体定义
struct GameState
{
    StateEnterFunc enter;
    StateExitFunc exit;
    StateUpdateFunc update;
    StateRenderFunc render;

    void* data;
    size_t datasize;
};

//状态机管理器
struct GameStateMachine
{
    GameState* current_state = nullptr;  // 当前状态
    GameState* pending_state = nullptr;  // 下一个状态
};

//状态机操作函数
void GameStateMachine_init(GameStateMachine* sm);
void GameStateMachine_change(GameStateMachine* sm, GameContext* ctx, GameState* newstate);
void GameStateMachine_update(GameStateMachine* sm, GameContext* ctx, float dt);
void GameStateMachine_render(GameStateMachine* sm, GameContext* ctx);
void GameStateMachine_shutdown(GameStateMachine* sm, GameContext* ctx);

//状态创建与销毁
GameState* Gamestate_create(
    StateEnterFunc enter,
    StateExitFunc exit,
    StateUpdateFunc update,
    StateRenderFunc render,
    void* data,
    size_t data_size
);
void GameState_destory(GameState* state);

//===============================================================================
// 【核心】GameContext 结构体 - 包含游戏所有资源
//===============================================================================
struct GameContext
{
    // 字体资源
    Font mainFont;
    
    // 玩家相关
    Player player;
    
    int width;
    int height;
    int tileSize;
    vector<vector<TileType>> tiles;                     // 地图数据
    unordered_map<TileType, Texture2D> mapTextures;     // 地图资产

    vector<Enemy> enemies;              //敌人列表
    Enemy* currentCombatant;            //当前战斗的敌人指针
    
    // 摄像机
    Camera2D camera;
    
    // 状态机
    GameStateMachine state_machine;
    
    // 窗口配置
    float screenWidth = 1920.0f;
    float screenHeight = 1080.0f;
    bool isRunning = true;
};

#endif // GAMESTATE_H