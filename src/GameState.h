/*
2025/11/13 Waring开始编写 游戏状态机
2025/11/14 wyt 修改了头文件保护使其正常生效
*/
#ifndef GAMESTATE_H //头文件保护
#define GAMESTATE_H 

//! 在下面引入其它库

#include <stddef.h> //引入size_t
#include "raylib.h"//引入raylib库以使用font字体文件
#include "Player.h"

//引入常用库
#include <vector>
#include <map>
#include <string>

//! 在上面引入其它库

#ifdef __cplusplus                      //这段代码在c++环境下编译
extern "C" {                            //使编译器以c风格调用函数而不是c++风格 因为c++支持函数重载而c不支持 c++编译器会对函数进行名称修饰
#endif

//前向声明
typedef struct GameContext GameContext;//不透明指针 包含游戏数据
typedef struct GameState GameState;

//状态生命周期函数指针
typedef void (*StateEnterFunc)(GameContext* ctx, void* state_data);         //初始化
typedef void (*StateExitFunc)(GameContext* ctx, void* state_data);          //退出
typedef void (*StateUpdateFunc)(GameContext* ctx, void* state_data);        //更新
typedef void (*StateRenderFunc)(GameContext* ctx, void* state_data);        //渲染

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
    GameState* current_state; //当前状态
    GameState* pending_state; //下一个状态
};

//=====================================状态机操作函数================================
void GameStateMachine_init(GameStateMachine* sm);                                                   //状态机初始化
void GameStateMachine_change(GameStateMachine* sm, GameContext* ctx, GameState* newstate);          //状态切换
void GameStateMachine_update(GameStateMachine* sm, GameContext* ctx, float dt);                     //状态更新
void GameStateMachine_render(GameStateMachine* sm, GameContext* ctx);                               //渲染当前状态
void GameStateMachine_shutdown(GameStateMachine* sm, GameContext* ctx);                             //关闭状态机

//状态创建===================================================
GameState* Gamestate_create(
    StateEnterFunc enter,
    StateExitFunc exit,
    StateUpdateFunc update,
    StateRenderFunc render,
    void* data,
    size_t data_size
);
//状态销毁===================================================
void GameState_destory(GameState* state);

#ifdef __cplusplus
}  // 结束 extern "C" 块
#endif

using namespace std;

//===============================================================================
// 【核心】
//  GameContext 结构体 包含此游戏所有资源
// 其他人以后可以在这里添加他需要的所有东西
struct GameContext {                                        //核心数据库  某个小可爱写错变量名了，记得不要手滑写成content  
    
    Font mainFont;                                          // 这是"字体加载器"唯一需要的东西：
    Player player;                                          //玩家属性结构体
    vector<vector<int>> currentMapData;                     //当前地图数据
    Camera2D camera;                                        //raylib 的 2D 摄像机结构体

    // 可以在这里添加...
    // enum GameState currentState;
    // ...等等

    //状态机
    GameStateMachine state_machine;
    //窗口配置
    int screenWidth;
    int screenHeight;
    bool isRunning;
};
//================================================================================

#endif // GAMESTATE_H - 头文件保护结束，确保此文件内容只被包含一次