#ifndef MAP_H       //保护头文件

#pragma once        // 防止重复包含

#include <vector>
#include <string>
#include "raylib.h"
#include <unordered_map>  // 新增：用于存储纹理

// 地块类型枚举
typedef enum {
    EMPTY,
    WALL,
    GRASS
} TileType;

// 地图结构体声明
typedef struct Map{
    int width;
    int height;
    int tileSize;
    vector<std::vector<TileType>> tiles;                    //存储所有地块类型
    unordered_map<TileType, Texture2D> mapTextures;         //地图资产
} Map;

// C风格函数声明（第一个参数是结构体指针）
void InitMap(Map* map, int width, int height, int tileSize);        //初始化地图
void CleanupMap(Map* map);                                          //清理地图资源
bool LoadMap(Map* map, const char* filepath);                       //加载地图
void LoadMapTextures(Map* map);                                     //加载纹理
void DrawMap(Map* map);                                             //绘图
void DrawSingleTile(Map* map, int tileX, int tileY); //绘制单个块
bool UpdateMap(Map* map);                                           //更新地图

// Getter函数
int GetMapWidth(const Map* map);
int GetMapHeight(const Map* map);
int GetMapTileSize(const Map* map);

#endif // MAP_H
