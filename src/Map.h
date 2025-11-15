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
    int width = 60;
    int height = 33;
    int tileSize = 32;
    std::vector<std::vector<TileType>> tiles;//存储所有地块类型

    //成员函数声明
    Map(int width,int height,int tileSize);
     ~Map();
    bool Load(const char* filepath);
    void Draw();
    void DrawSingleTile(int tileX, int tileY, TileType type, int tileSize);
    bool Update();
    bool CheckCollision(Vector2 position);

    // Getter接口
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    int GetTileSize() const { return tileSize; }

} Map;

#endif // MAP_H
