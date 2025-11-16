#include "Player.h"
#include "raylib.h"
#include "GameState.h"
#include "Map.h"//TODO 需等待map完成

const int TILE_SIZE = 32;//标准图块大小

 /**
  * @brief 用于读取键盘输入来更新玩家位置
  * 
  * @param ctx Gamestate里的GameContext核心数据文件
  */
void updatePlayer(GameContext& ctx){
    //初始化移动意图
    int nextX = ctx.player.gridX;
    int nextY = ctx.player.gridY;

    //将按键转化为移动意图
    if(IsKeyPressed(KEY_W)){//当W键按下 一次 时，仅一次向上移动一格
        nextY--;//*注意，向上是减少Y坐标
    }
    else if(IsKeyPressed(KEY_S)){//当S键按下 一次 时，仅一次向下移动一格
        nextY++;//*注意，向下是增加Y坐标
    }
    else if(IsKeyPressed(KEY_A)){//当A键按下 一次 时，仅一次向左移动一格
        nextX--;
    }
    else if(IsKeyPressed(KEY_D)){//当D键按下 一次 时，仅一次向右移动一格
        nextX++;
    }

    //------开始进行碰撞检测------
    //TODO 需等待map完成
    //假设engine.currentMapData[y][x]存在，他的声明如下
    //vector<vector<int>> currentMapData;
    //假设0为可移动地板

    bool canMove = false; //初始化为：不可移动

    //【修复】安全检查顺序：先检查 Y，再用该行的实际宽度检查 X
    //这样可以处理"锯齿状"地图（不同行宽度不同的情况）

    //检查地图是否为空
    if(ctx.currentMapData.empty()){
        TraceLog(LOG_FATAL,"[PlayerMove] Map data is empty!");
        canMove = false;
    }
    //【第一步】检测 Y 轴是否超出地图限制
    else if(nextY < 0 || nextY >= (int)ctx.currentMapData.size()){
        TraceLog(LOG_WARNING,"[PlayerMove] Invalid move attempt! Out of map bounds of Y. Target position:(%d,%d)",nextX,nextY);
        canMove = false;
    }
    //【第二步】Y 轴安全，现在用 nextY 这一行来检查 X 的实际宽度
    else if(ctx.currentMapData[nextY].empty()){
        TraceLog(LOG_WARNING,"[PlayerMove] Invalid move attempt! Row %d is empty. Target position:(%d,%d)",nextY,nextX,nextY);
        canMove = false;
    }
    else if(nextX < 0 || nextX >= (int)ctx.currentMapData[nextY].size()){
        TraceLog(LOG_WARNING,"[PlayerMove] Invalid move attempt! Out of map bounds of X. Target position:(%d,%d)",nextX,nextY);
        canMove = false;
    }
    //【第三步】X 和 Y 都安全，检测目标位置是否可移动
    else if(ctx.currentMapData[nextY][nextX] != 0){
        TraceLog(LOG_WARNING,"[PlayerMove] Invalid move attempt! Target tile is not walkable. Target position:(%d,%d)",nextX,nextY);
        canMove = false;
    }
    else{
        //所有检查通过，可以移动
        canMove = true;
    }
    //------碰撞检测结束------

    //------开始执行移动------
    if(canMove){
    ctx.player.gridX = nextX;
    ctx.player.gridY = nextY;
    }
    //------移动结束------
}

/**
 * @brief 角色摄像机，负责绘制非战斗情况，即大地图时的画面
 * 
 * @param ctx Gamestate里的GameContext核心数据文件
 */
void drawPlayer(const GameContext& ctx){
    Camera2D camera = ctx.camera;//创建一个camera的副本

    //------开始设置角色摄像机------
    camera.zoom = 3.0f;//? 为啥要加f--->因为zoom本身就是个float量，3.0这样会默认为double
    camera.offset = {1920/2.0f, 1080/2.0f};
    camera.target = { 
        (float)(ctx.player.gridX * TILE_SIZE) + (TILE_SIZE / 2.0f),
        (float)(ctx.player.gridY * TILE_SIZE) + (TILE_SIZE / 2.0f)
    };
    //------结束角色摄像机设置------

    //------开始角色摄像机绘制------
    BeginMode2D(camera);

    //绘制地图
    DrawMap();//TODO 这个函数的参数有问题，待修改正确后再写入参数

    //绘制角色
    DrawRectangle(ctx.player.gridX * TILE_SIZE, ctx.player.gridY * TILE_SIZE,TILE_SIZE,TILE_SIZE,RED);

    //结束角色渲染，恢复到正常尺寸以备之后绘制ui
    EndMode2D();
    //------结束角色摄像机绘制------

    //TODO P1阶段时，可以在这之后绘制UI

}