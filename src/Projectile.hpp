#pragma once
#include <SFML/Graphics.hpp>

struct Projectile
{
    sf::RectangleShape shape;
    sf::Vector2f velocity;

    Projectile(float x, float y, float dirX, float dirY)
    {
        shape.setSize(sf::Vector2f(5.0f, 15.0f)); // Витягнута куля
        shape.setFillColor(sf::Color::Yellow);
        shape.setOrigin(2.5f, 7.5f);
        shape.setPosition(x, y);
        
        float speed = 20.0f;
        velocity = sf::Vector2f(dirX * speed, dirY * speed);
    }

    void update()
    {
        shape.move(velocity);
    }

    void draw(sf::RenderWindow& window)
    {
        window.draw(shape);
    }
    
    bool isOutOfBounds(float width, float height)
    {
        sf::Vector2f pos = shape.getPosition();
        return (pos.y < -50 || pos.y > height + 50);
    }
    
    sf::FloatRect getBounds() const { return shape.getGlobalBounds(); }
};