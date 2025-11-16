#include "Player.h"
#include "raylib.h"
#include "GameState.h"
#include "Map.h"//TODO 需等待map完成

const int TILE_SIZE = 32;//标准图块大小
const char* playerSpriteAddress = "res/graphics/player/Pixel_Taffy/Pixel_Taffy_Sprite.png";//玩家精灵位置

/**
 * @brief (P1) 加载玩家所需的美术资源 (贴图)
 * (这个函数必须在 main() 的"初始化"中被调用)
 * @param ctx 
 */
void LoadPlayerAssets(GameContext& ctx){
    // 尝试多个可能的路径（支持从build目录或项目根目录运行）
    const char* possiblePaths[] = {
        "res/graphics/player/Pixel_Taffy/Pixel_Taffy_Sprite.png",
        "../res/graphics/player/Pixel_Taffy/Pixel_Taffy_Sprite.png",
        "../../res/graphics/player/Pixel_Taffy/Pixel_Taffy_Sprite.png"
    };
    
    bool loaded = false;
    for (int i = 0; i < 3; i++) {
        ctx.player.spriteSheet = LoadTexture(possiblePaths[i]);
        if(ctx.player.spriteSheet.id != 0){
            TraceLog(LOG_INFO, "[Player] Sprite sheet loaded successfully: %s (ID: %d, Size: %dx%d)", 
                     possiblePaths[i], 
                     ctx.player.spriteSheet.id,
                     ctx.player.spriteSheet.width,
                     ctx.player.spriteSheet.height);
            loaded = true;
            break;
        }
    }
    
    if(!loaded){
        TraceLog(LOG_ERROR, "[Player] Failed to load sprite sheet from all paths");
        TraceLog(LOG_ERROR, "[Player] Current working directory: %s", GetWorkingDirectory());
        TraceLog(LOG_ERROR, "[Player] Will use red rectangle as placeholder");
    }
}

/**
 * @brief (P1) 卸载玩家的美术资源
 * (这个函数必须在 main() 的“清理”中被调用)
 * @param ctx 
 */
void UnloadPlayerAssets(GameContext& ctx){
    if(ctx.player.spriteSheet.id != 0){
        UnloadTexture(ctx.player.spriteSheet);
    }
    else{
        TraceLog(LOG_ERROR,"[Player Texture Unloader] Trying to unload an empty texture!");
    }
}

 /**
  * @brief 用于读取键盘输入来更新玩家位置
  * 
  * @param ctx Gamestate里的GameContext核心数据文件
  */
void updatePlayer(GameContext& ctx){


    //------开始移动状态更新------

    //初始化移动意图
    int nextX = ctx.player.gridX;
    int nextY = ctx.player.gridY;

    //将按键转化为移动意图
    if(IsKeyPressed(KEY_W)){//当W键按下 一次 时，仅一次向上移动一格
        nextY--;//*注意，向上是减少Y坐标
        ctx.player.currentDirection = PLAYER_DIR_UP;
    }
    else if(IsKeyPressed(KEY_S)){//当S键按下 一次 时，仅一次向下移动一格
        nextY++;//*注意，向下是增加Y坐标
        ctx.player.currentDirection = PLAYER_DIR_DOWN;
    }
    else if(IsKeyPressed(KEY_A)){//当A键按下 一次 时，仅一次向左移动一格
        nextX--;
        ctx.player.currentDirection = PLAYER_DIR_LEFT;
    }
    else if(IsKeyPressed(KEY_D)){//当D键按下 一次 时，仅一次向右移动一格
        nextX++;
        ctx.player.currentDirection = PLAYER_DIR_RIGHT;
    }

    //------开始进行碰撞检测------

    bool canMove = false; //初始化为：不可移动

    //【修复】安全检查顺序：先检查 Y，再用该行的实际宽度检查 X
    //这样可以处理"锯齿状"地图（不同行宽度不同的情况）

    //检查地图是否为空
    if(ctx.tiles.empty()){
        TraceLog(LOG_FATAL,"[PlayerMove] Map data is empty!");
        canMove = false;
    }
    //【第一步】检测 Y 轴是否超出地图限制
    else if(nextY < 0 || nextY >= (int)ctx.tiles.size()){
        TraceLog(LOG_WARNING,"[PlayerMove] Invalid move attempt! Out of map bounds of Y. Target position:(%d,%d)",nextX,nextY);
        canMove = false;
    }
    //【第二步】Y 轴安全，现在用 nextY 这一行来检查 X 的实际宽度
    else if(ctx.tiles[nextY].empty()){
        TraceLog(LOG_WARNING,"[PlayerMove] Invalid move attempt! Row %d is empty. Target position:(%d,%d)",nextY,nextX,nextY);
        canMove = false;
    }
    else if(nextX < 0 || nextX >= (int)ctx.tiles[nextY].size()){
        TraceLog(LOG_WARNING,"[PlayerMove] Invalid move attempt! Out of map bounds of X. Target position:(%d,%d)",nextX,nextY);
        canMove = false;
    }
    //【第三步】X 和 Y 都安全，检测目标位置是否可移动
    else if(ctx.tiles[nextY][nextX] != EMPTY && ctx.tiles[nextY][nextX] != GRASS){
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

    //------移动状态更新结束------


    //------开始战斗状态更新------
    //? 战斗状态进入应该写在这吗？

    if(IsKeyPressed(KEY_E)){
        //TODO 在这里进入战斗状态
    }

    //------结束战斗状态更新------
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
    camera.offset = {ctx.screenWidth/2.0f, ctx.screenHeight/2.0f}; // 摄像机偏移量，设置为屏幕中心（屏幕宽高的一半）
    camera.target = { // 摄像机目标位置，指向玩家所在瓦片的中心点
        (float)(ctx.player.gridX * TILE_SIZE) + (TILE_SIZE / 2.0f), // 玩家网格X坐标 * 瓦片大小 + 瓦片中心偏移
        (float)(ctx.player.gridY * TILE_SIZE) + (TILE_SIZE / 2.0f)  // 玩家网格Y坐标 * 瓦片大小 + 瓦片中心偏移
    };
    //------结束角色摄像机设置------

    //------开始角色摄像机绘制------
    BeginMode2D(camera);

    //绘制地图
    DrawMap(ctx);

    //绘制角色
    Rectangle source;//声明绘制源
    source.width = 32.0f;
    source.height = 64.0f;
    source.y = 0.0f;


    // 根据“当前方向” (已在 updatePlayer 设置)
    // 修改 source.x 来“裁剪”正确的帧
    switch (ctx.player.currentDirection)
    {
        // “合同”：第 1 帧 (x=0) 是 "Left"
        case PLAYER_DIR_LEFT:
            source.x = 0.0f;   // 0 * 32
            break;
            
        // “合同”：第 2 帧 (x=32) 是 "Down"
        case PLAYER_DIR_DOWN:
            source.x = 32.0f;  // 1 * 32
            break;                
            // “合同”：第 3 帧 (x=64) 是 "Right"
        case PLAYER_DIR_RIGHT:
            source.x = 64.0f;  // 2 * 32
            break;
        // "合同"：第 4 帧 (x=96) 是 "Up"
        case PLAYER_DIR_UP:
            source.x = 96.0f;  // 3 * 32
            break;
        
        // [P1 Safety Net] Prevent rendering bugs from uninitialized or garbage values
        default:
            source.x = 32.0f;  // Default to Down direction
            TraceLog(LOG_WARNING, "[Player] Abnormal currentDirection value detected: %d, using default direction", ctx.player.currentDirection);
            break;
    }

    //必须"向上偏移" 1 格 (TILE_SIZE)
    // 这样"32x64"的"脚"才能"踩"在"32x32"的 P0 格子上
    Vector2 drawDestPosition = {//仅渲染用坐标
        (float)ctx.player.gridX * TILE_SIZE,
        (float)ctx.player.gridY * TILE_SIZE - TILE_SIZE // 【【【向上偏移！】】】
    };

    // 【调试】绘制玩家精灵图
    if(ctx.player.spriteSheet.id != 0){
        // 纹理加载成功，绘制精灵图
        DrawTextureRec(ctx.player.spriteSheet, source, drawDestPosition, WHITE);
    } else {
        // 【备用方案】纹理未加载，绘制红色方块作为占位符
        DrawRectangle(
            ctx.player.gridX * TILE_SIZE, 
            ctx.player.gridY * TILE_SIZE - TILE_SIZE,
            32, 64, RED
        );
        DrawText("NO SPRITE", ctx.player.gridX * TILE_SIZE, ctx.player.gridY * TILE_SIZE, 10, WHITE);
    }

    //结束角色渲染，恢复到正常尺寸以备之后绘制ui
    EndMode2D();
    //------结束角色摄像机绘制------

    //TODO P1阶段时，可以在这之后绘制UI

}