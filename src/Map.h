#ifndef MAP_H
#define MAP_H

#include "raylib.h"

// 地图系统类
class Map {
private:
    int width;
    int height;
    int tileSize;
    
public:
    Map(int w, int h, int size);
    ~Map();
    
    void Load(const char* filepath);
    void Draw();
    void Update();
    bool CheckCollision(Vector2 position);
};

#endif // MAP_H
