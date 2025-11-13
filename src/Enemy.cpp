#include "Enemy.h"

Enemy::Enemy(const char* enemyName, Vector2 pos) : name(enemyName), position(pos) {
    // 初始化敌人属性
    hp = 50;
    maxHp = 50;
    attack = 8;
    defense = 3;
}

Enemy::~Enemy() {
    // 析构函数：清理资源
}

void Enemy::Update() {
    // TODO: 更新敌人状态
}

void Enemy::Draw() {
    // TODO: 绘制敌人角色
    DrawCircleV(position, 16, RED);
}

void Enemy::TakeDamage(int damage) {
    // 受到伤害
    int actualDamage = damage - defense;
    if (actualDamage < 0) actualDamage = 0;
    hp -= actualDamage;
    if (hp < 0) hp = 0;
}

int Enemy::Attack() {
    // 返回攻击力
    return attack;
}
