#pragma once
#include "Item.hpp"
#include <vector>
#include <SFML/Graphics.hpp>

class Inventory
{
public:
    Inventory();
    
    void addItem(const Item& item);
    void setFont(const sf::Font& font);
    void draw(sf::RenderWindow& window);

    // --- НОВІ ФУНКЦІЇ ДЛЯ УПРАВЛІННЯ ---
    void selectNext(); // Вибрати наступний
    void selectPrev(); // Вибрати попередній
    bool hasItems() const;
    Item getItemAndRemove(); // Використати предмет (повертає його і видаляє)

private:
    std::vector<Item> items;
    int selectedIndex; // Який предмет зараз виділено
    
    sf::Font inventoryFont;
    sf::RectangleShape background;
    sf::Text titleText;
};