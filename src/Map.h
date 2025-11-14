#ifndef MAP_H
#define MAP_H

#pragma once  // 防止重复包含

#include <vector>
#include <string>
#include "raylib.h"
/**
 * @brief 不同地块的类型
 * 
 */
enum class TileType {
    EMPTY,      // 空地
    WALL,       // 墙壁
    GRASS       // 草地
};

/**
 * @brief 地图系统类
 * 
 */
class Map {
private:
    // 核心数据：存储每个块的类型（必须有！）
    std::vector<std::vector<TileType>> tileMap; 

    int width = 40;              //宽
    int height = 22;             //高
    int tileSize = 32;           //单位方块像素数
    
public:
    Map(int w, int h, int size);                //初始化地图
    ~Map();                                     //释放地图
    
    bool Load(const char* filepath);            //加载地图
    void Draw();                                //画图
    bool Update();                              //更新地图
    bool CheckCollision(Vector2 position);      //检测碰撞

    // Getter函数：让外部模块安全访问私有数据
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    int GetTileSize() const { return tileSize; }

};

#endif // MAP_H
