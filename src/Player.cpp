#include "Player.h"
#include "raylib.h"
#include "GameState.h"
#include "Map.h"//TODO 需等待map完成

const int TILE_SIZE = 32;//标准图块大小

 /**
  * @brief 用于读取键盘输入来更新玩家位置
  * 
  * @param engine Gamestate里的engine核心数据文件
  */
void updatePlayer(GameEngine& engine){
    //初始化移动意图
    int nextX = engine.player.gridX;
    int nextY = engine.player.gridY;

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

    //检查地图是否为空
    if(engine.currentMapData.empty() || engine.currentMapData[0].empty()){
        TraceLog(LOG_FATAL,"[PlayerMove] Map data is empty!");
        canMove = false;
    }
    //检测是否超出地图限制
    else if(nextY < 0 || nextY >= (int)engine.currentMapData.size()){
        TraceLog(LOG_WARNING,"[PlayerMove] Invalid move attempt! Out of map bounds of Y. Target position:(%d,%d)",nextX,nextY);
        canMove = false;
    }
    else if(nextX < 0 || nextX >= (int)engine.currentMapData[0].size()){
        TraceLog(LOG_WARNING,"[PlayerMove] Invalid move attempt! Out of map bounds of X. Target position:(%d,%d)",nextX,nextY);
        canMove = false;
    }
    //检测目标位置是否可移动
    else if(engine.currentMapData[nextY][nextX] != 0){
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
    engine.player.gridX = nextX;
    engine.player.gridY = nextY;
    }
    //------移动结束------
}

/**
 * @brief 角色摄像机，负责绘制非战斗情况，即大地图时的画面
 * 
 * @param engine Gamestate里的engine核心数据文件
 */
void drawPlayer(const GameEngine& engine){
    Camera2D camera = engine.camera;//创建一个camera的副本

    //------开始设置角色摄像机------
    camera.zoom = 3.0f;//?为啥要加f--->因为zoom本身就是个float量，3.0这样会默认为double
    camera.offset = {1920/2.0f, 1080/2.0f};
    camera.target = { 
        (float)(engine.player.gridX * TILE_SIZE) + (TILE_SIZE / 2.0f),
        (float)(engine.player.gridY * TILE_SIZE) + (TILE_SIZE / 2.0f)
    };
    //------结束角色摄像机设置------

    //------开始角色摄像机绘制------
    BeginMode2D(camera);

    //绘制地图
    DrawMap();//TODO 这个函数的参数有问题，待修改正确后再写入参数

    //绘制角色
    DrawRectangle(engine.player.gridX * TILE_SIZE, engine.player.gridY * TILE_SIZE,TILE_SIZE,TILE_SIZE,RED);

    //结束角色渲染，恢复到正常尺寸以备之后绘制ui
    EndMode2D();
    //------结束角色摄像机绘制------

    //TODO P1阶段时，可以在这之后绘制UI

}