/*
2025/11/13 Waring开始编写 游戏状态机
2025/11/14 wyt 修改了头文件保护使其正常生效
*/
#ifndef GAMESTATE_H //头文件保护
#define GAMESTATE_H 

//! 在下面引入其它库

#include <stddef.h> //引入size_t
#include "raylib.h"//引入raylib库以使用font字体文件

//引入常用库
#include <vector>
#include <map>
#include <string>

//! 在上面引入其它库

#ifdef __cplusplus //这段代码在c++环境下编译
extern "C" {  //使编译器以c风格调用函数而不是c++风格 因为c++支持函数重载而c不支持 c++编译器会对函数进行名称修饰
#endif

//前向声明
typedef struct GameContext GameContext;//不透明指针 包含游戏数据
typedef struct GameState GameState;

typedef void (*StateEnterFunc)(GameContext* ctx, void* state_data);
typedef void (*StateExitFunc)(GameContext* ctx, void* state_data);
typedef void (*StateUpdateFunc)(GameContext* ctx, void* state_data);

#ifdef __cplusplus
}  // 结束 extern "C" 块
#endif

// 【核心】
// 先定义一个 GameEngine 结构体
// 其他人以后可以在这里添加他需要的所有东西
struct GameEngine {
    
    Font mainFont; // 这是"字体加载器"唯一需要的东西：
    Player player;//玩家属性结构体
    // 可以在这里添加...
    // enum GameState currentState;
    // Player* player;
    // Camera2D camera;
    // vector<vector<int>> currentMapData;
    // ...等等
};

#endif // GAMESTATE_H - 头文件保护结束，确保此文件内容只被包含一次