#include "Enemy.hpp"
#include <cmath>

Enemy::Enemy(float startX, float startY, EnemyType type, const sf::Texture& texture)
    : type(type)
{
    if (texture.getSize().x > 0) {
        shape.setTexture(&texture);
    } else {
        shape.setFillColor(sf::Color::Red);
    }

    shape.setPosition(startX, startY);

    switch (type) {
    case EnemyType::Small:
        shape.setSize(sf::Vector2f(30.f, 30.f));
        speed = 0.5f;
        hp = 1;
        break;
    case EnemyType::Medium:
        shape.setSize(sf::Vector2f(50.f, 50.f));
        speed = 0.5f;
        hp = 3;
        break;
    case EnemyType::Tank:
        shape.setSize(sf::Vector2f(80.f, 80.f));
        speed = 0.5f;
        hp = 10;
        break;
    }
    
    shape.setOrigin(shape.getSize().x / 2, shape.getSize().y / 2);
}

void Enemy::update(sf::Vector2f playerPos)
{
    sf::Vector2f enemyPos = shape.getPosition();
    float dx = playerPos.x - enemyPos.x;
    float dy = playerPos.y - enemyPos.y;
    
    float distance = std::sqrt(dx*dx + dy*dy);
    
    if (distance > 0)
    {
        float moveX = (dx / distance) * speed;
        float moveY = (dy / distance) * speed;
        shape.move(moveX, moveY);
    }
    
    float angle = std::atan2(dy, dx) * 180 / 3.14159f;
    shape.setRotation(angle + 90);
}

void Enemy::draw(sf::RenderWindow& window)
{
    window.draw(shape);
}

sf::FloatRect Enemy::getBounds() const
{
    return shape.getGlobalBounds();
}

bool Enemy::takeDamage(int damage)
{
    hp -= damage;
    return (hp <= 0);
}

EnemyType Enemy::getType() const
{
    return type;
}

// --- НОВІ МЕТОДИ ---
sf::Vector2f Enemy::getPosition() const {
    return shape.getPosition();
}

void Enemy::setPosition(float x, float y) {
    shape.setPosition(x, y);
}

void Enemy::move(float x, float y) {
    shape.move(x, y);
}