#include "Combat.h"

Combat::Combat() : isActive(false), turn(0) {
    // 构造函数：初始化战斗系统
}

Combat::~Combat() {
    // 析构函数：清理资源
}

void Combat::Start(Player* player, Enemy* enemy) {
    // TODO: 开始战斗
    isActive = true;
    turn = 0;
}

void Combat::Update() {
    // TODO: 更新战斗逻辑
}

void Combat::Draw() {
    // TODO: 绘制战斗界面
}

void Combat::End() {
    // TODO: 结束战斗
    isActive = false;
}

bool Combat::IsActive() {
    return isActive;
}
