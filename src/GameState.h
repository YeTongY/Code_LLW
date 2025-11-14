/*
2025/11/13 Waring开始编写 游戏状态机
*/
#ifndef GAMESTATE_H //头文件保护
#define GAMESTATE_H 

#include <stddef.h> //引入size_t

#ifdef __cplusplus //这段代码在c++环境下编译
extern "C" {  //使编译器以c风格调用函数而不是c++风格 因为c++支持函数重载而c不支持 c++编译器会对函数进行名称修饰
#endif

//前向声明
typedef struct GameContext GameContext;//不透明指针 包含游戏数据
typedef struct GameState GameState;

typedef void (*StateEnterFunc)(GameContext* ctx, void* state_data);
typedef void (*StateExitFunc)(GameContext* ctx, void* state_data);
typedef void (*StateUpdateFunc)(GameContext* ctx, void* state_data);

}
#endif // GAMESTATE_H