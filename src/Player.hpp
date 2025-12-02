#pragma once
#include <SFML/Graphics.hpp>
#include "Inventory.hpp"

class Player
{
public:
    Player(float startX, float startY);
    
    void handleEvent(const sf::Event& event);
    
  void update();
    
    void draw(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;

    Inventory inventory;
    sf::RectangleShape shape; 

private:
    sf::Vector2f velocity;      // Поточна швидкість
    float acceleration;         // Сила розгону
    float drag;                 // Тертя (інерція)
    float maxSpeed;             // Максимальна швидкість

    sf::Texture playerTexture;

    bool isMovingLeft;
    bool isMovingRight;
    bool isMovingUp;
    bool isMovingDown;
};