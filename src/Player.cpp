#include "Player.h"

Player::Player() {
    // 初始化玩家属性
    position = { 400, 300 };
    hp = 100;
    maxHp = 100;
    attack = 10;
    defense = 5;
    speed = 2.0f;
}

Player::~Player() {
    // 析构函数：清理资源
}

void Player::Update() {
    // TODO: 更新玩家状态
}

void Player::Draw() {
    // TODO: 绘制玩家角色
    DrawCircleV(position, 16, BLUE);
}

void Player::Move(Vector2 direction) {
    // TODO: 移动玩家
    position.x += direction.x * speed;
    position.y += direction.y * speed;
}

void Player::TakeDamage(int damage) {
    // 受到伤害
    int actualDamage = damage - defense;
    if (actualDamage < 0) actualDamage = 0;
    hp -= actualDamage;
    if (hp < 0) hp = 0;
}

void Player::Heal(int amount) {
    // 恢复生命值
    hp += amount;
    if (hp > maxHp) hp = maxHp;
}
