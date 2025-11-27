#ifndef MAP_H       //保护头文件

#pragma once        // 防止重复包含

#include <vector>
#include <string>
#include "raylib.h"
#include <unordered_map>  // 新增：用于存储纹理

struct GameContext;

// 地块类型枚举
typedef enum {
    EMPTY = 0,
    GRASS = 1,
    WALL = 2
} TileType;

// 地图结构体声明
//typedef struct Map{
    //int width;
    //int height;
    //int tileSize;
    //vector<std::vector<TileType>> tiles;                    //存储所有地块类型
    //unordered_map<TileType, Texture2D> mapTextures;         //地图资产
//} Map;

// C风格函数声明（第一个参数是结构体指针）
void CleanupMap(GameContext& map);                                          //清理地图资源
void LoadMapTextures(GameContext& map);                                     //加载图块集
void DrawMap(const GameContext& map);                                       //绘图
void DrawGroundLayers(const GameContext& map);
void DrawYSortLayer(const GameContext& map);
void DrawOverheadLayers(const GameContext& map);
void DrawSingleTile(const GameContext& map, int tileX, int tileY,unsigned int gid);          //绘制单个块
bool UpdateMap(GameContext& map);                                           //更新地图

bool LoadLevelFromTiled(GameContext& ctx, const char* filepath);            // 替换旧的 LoadMap

// Getter函数
int GetMapWidth(const GameContext& map);
int GetMapHeight(const GameContext& map);
int GetMapTileSize(const GameContext& map);

#endif // MAP_H
