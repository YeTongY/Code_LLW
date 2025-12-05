/*
2025/11/13 Warning开始编写 游戏状态机
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
#include "Event.h"
#include "UI.h" // 新增：需要 HUDTemplate 定义
//================================================================================
//颜色定义
// 1. “主要”的“赛博朋克”辉光色 (明亮的青色)
// (我们不“完全”用 0,255,255，我们用一个更“柔和”的青色)
#define CYBER_CYAN (Color){ 0, 228, 228, 255 }

// 2. “主要”的 UI 背景色 (非常暗的青色)
#define UI_BACKGROUND (Color){ 10, 40, 45, 255 }

// 3. “主要”的 UI 文字色 (干净的白色)
#define UI_TEXT (Color){ 255, 255, 255, 255 }

// 4. “P1 锦上添花”色 (用于 HP, MP, 警告)
#define CYBER_LIME (Color){ 0, 255, 128, 255 }  // 亮绿色
#define CYBER_MAGENTA (Color){ 255, 0, 255, 255 } // 品红/粉色
#define CYBER_RED (Color){ 255, 50, 50, 255 }  // 亮红色
//================================================================================

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
    vector<vector<TileType>> tiles;                     // 地图逻辑数据（碰撞检测用）
    vector<vector<vector<unsigned int>>> tileGIDs;     // Tiled 原始 GID 数据（绘制用）
    vector<vector<vector<unsigned int>>> groundLayers; // 背景/地板层
    vector<vector<vector<unsigned int>>> overheadLayers;// 前景遮挡层
    vector<vector<vector<unsigned int>>> decorationLayers; // 装饰层（备用）
    unordered_map<TileType, Texture2D> mapTextures;     // 地图资产
    vector<Texture2D> tilesetTextures;                  // Tiled tileset 纹理列表
    vector<int> tilesetFirstGIDs;                       // 每个 tileset 的起始 GID

    Enemy* currentCombatant;            //当前战斗的敌人指针
    string pendingPreCombatDialogue;    // 即将触发的战斗前对话脚本
    string pendingPostCombatDialogue;   // 即将触发的战斗后对话脚本
    vector<Enemy> enemies;
    Texture2D enemySpriteSheet;
    map<string, Texture2D> enemyTextures;
    
    // 摄像机
    Camera2D camera;

    //滤镜
    Shader cyberpunkShader;          // “赛博朋克”滤镜
    RenderTexture2D mapRenderTexture; // “P1 渲染画布”
    
    // 状态机
    GameStateMachine state_machine;
    
    // 窗口配置
    float screenWidth = 1920.0f;
    float screenHeight = 1080.0f;
    bool isRunning = true;

    //UI纹理
    Texture2D dialogueBoxTexture;
    Texture2D portraitFrameTexture;

    //大地图玩家头像
    Texture2D playerPortrait;
    Texture2D playerPortraitFrame;

    // UI 模板实例
    HUDTemplate hudTemplate;

    bool enableFootstepAudio = false; // 探索状态控制脚步音效
    bool isFootstepLooping = false;   // 脚步声循环标记
    float footstepIdleTimer = 0.0f;   // 停止前的缓冲计时

    bool isExplorationBGMPlaying = false; // 探索背景音乐播放标记
    bool isCombatBGMPlaying = false;      // 战斗背景音乐播放标记
    bool level0CombatDialogueTriggered = false; // Level0 战斗对话是否已触发

    //音效
    Music footstepLoop = {0};
    Music explorationBGM = {0};
    Music combatBGM = {0};

    //标题图片
    Texture2D titleScreen;
    Texture2D combatBackground;    // 战斗场景背景


    //静态事件列表
    vector <GameEvent> gameEvents;

};

#endif // GAMESTATE_H