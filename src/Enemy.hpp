#pragma once
#include <SFML/Graphics.hpp>

enum class EnemyType {
    Small,  // Швидкий
    Medium, // Середній
    Tank    // Жирний
};

class Enemy
{
public:
    Enemy(float startX, float startY, EnemyType type, const sf::Texture& texture);
    
    void update(sf::Vector2f playerPos);
    void draw(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
    bool takeDamage(int damage);
    EnemyType getType() const; 

    // --- НОВІ МЕТОДИ ДЛЯ КОЛІЗІЙ ---
    sf::Vector2f getPosition() const;
    void setPosition(float x, float y);
    void move(float x, float y);
    // -------------------------------

private:
    sf::RectangleShape shape;
    EnemyType type;
    float speed;
    int hp;
};