#include "Player.hpp"
#include <iostream>
#include <cmath>

Player::Player(float startX, float startY)
    : inventory()
{
    // Налаштування фізики
    velocity = sf::Vector2f(0.f, 0.f);
    acceleration = 0.5f; // Як швидко розганяється
    drag = 0.95f;        // Коефіцієнт ковзання (0.9 - лід, 0.5 - пісок)
    maxSpeed = 7.0f;    // Максималка

    isMovingLeft = false;
    isMovingRight = false;
    isMovingUp = false;
    isMovingDown = false;

    shape.setSize(sf::Vector2f(60.0f, 60.0f)); 
    shape.setOrigin(30.0f, 30.0f); // Центр
    shape.setPosition(startX, startY);

    if (!playerTexture.loadFromFile("res/textures/ship.png"))
    {
        shape.setFillColor(sf::Color::Cyan); 
    }
    else
    {
        shape.setTexture(&playerTexture);
    }
}

void Player::handleEvent(const sf::Event& event)
{
    // WASD або Стрілки
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::Left) isMovingLeft = true;
        if (event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::Right) isMovingRight = true;
        if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up) isMovingUp = true;
        if (event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down) isMovingDown = true;
    }
    
    if (event.type == sf::Event::KeyReleased) {
        if (event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::Left) isMovingLeft = false;
        if (event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::Right) isMovingRight = false;
        if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up) isMovingUp = false;
        if (event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down) isMovingDown = false;
    }
}

void Player::update()
{
    // 1. Застосування сили (розгін)
    if (isMovingLeft)  velocity.x -= acceleration;
    if (isMovingRight) velocity.x += acceleration;
    if (isMovingUp)    velocity.y -= acceleration;
    if (isMovingDown)  velocity.y += acceleration;

    // 2. Тертя (уповільнення, коли не тиснемо кнопки)
    velocity *= drag;

    // 3. Обмеження максимальної швидкості
    float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    if (speed > maxSpeed) {
        float scale = maxSpeed / speed;
        velocity *= scale;
    }

    // Зупинка при дуже малій швидкості
    if (std::abs(velocity.x) < 0.1f) velocity.x = 0.f;
    if (std::abs(velocity.y) < 0.1f) velocity.y = 0.f;

    // 4. Рух
    shape.move(velocity);

    // 5. Нахил корпусу при повороті
    float rotation = velocity.x * 2.0f;
    shape.setRotation(rotation);
}

void Player::draw(sf::RenderWindow& window)
{
    window.draw(shape);
}

sf::FloatRect Player::getBounds() const
{
    return shape.getGlobalBounds();
}

sf::Vector2f Player::getPosition() const
{
    return shape.getPosition();
}