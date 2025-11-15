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
    std::vector<std::vector<TileType>> tiles;//存储所有地块类型
} Map;

// C风格函数声明（第一个参数是结构体指针）
void InitMap(Map* map, int width, int height, int tileSize);
void CleanupMap(Map* map);
bool LoadMap(Map* map, const char* filepath);
void DrawMap(Map* map);
void DrawSingleTile(Map* map, int tileX, int tileY, TileType type);
bool UpdateMap(Map* map);
bool CheckMapCollision(Map* map, Vector2 position);

// Getter函数
int GetMapWidth(const Map* map);
int GetMapHeight(const Map* map);
int GetMapTileSize(const Map* map);

#endif // MAP_H
