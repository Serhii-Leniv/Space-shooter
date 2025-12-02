#include "Inventory.hpp"
#include <iostream>

Inventory::Inventory() : selectedIndex(0)
{
    background.setSize(sf::Vector2f(400, 500));
    background.setFillColor(sf::Color(50, 50, 50, 230)); // Більш прозорий
    background.setOutlineColor(sf::Color::Cyan); // Неонова рамка
    background.setOutlineThickness(3.0f);
    background.setPosition(200, 50); // Центр (для 800x600)

    titleText.setString("INVENTORY (Use: ENTER)");
    titleText.setCharacterSize(24);
    titleText.setFillColor(sf::Color::Cyan);
}

void Inventory::setFont(const sf::Font& font)
{
    inventoryFont = font;
    titleText.setFont(inventoryFont);
    
    sf::FloatRect bgBounds = background.getGlobalBounds();
    sf::FloatRect titleBounds = titleText.getGlobalBounds();
    titleText.setPosition(
        bgBounds.left + (bgBounds.width - titleBounds.width) / 2,
        bgBounds.top + 15
    );
}

void Inventory::addItem(const Item& item)
{
    items.push_back(item);
}

// --- НАВІГАЦІЯ ---
void Inventory::selectNext() {
    if (items.empty()) return;
    selectedIndex = (selectedIndex + 1) % items.size();
}

void Inventory::selectPrev() {
    if (items.empty()) return;
    selectedIndex = (selectedIndex - 1 + items.size()) % items.size();
}

bool Inventory::hasItems() const {
    return !items.empty();
}

Item Inventory::getItemAndRemove() {
    if (items.empty()) return Item("Empty", ItemType::ScoreBonus);
    
    // Беремо предмет
    Item item = items[selectedIndex];
    // Видаляємо його
    items.erase(items.begin() + selectedIndex);
    
    // Коригуємо курсор, щоб не вийти за межі
    if (selectedIndex >= items.size() && !items.empty()) {
        selectedIndex = items.size() - 1;
    }
    
    return item;
}

void Inventory::draw(sf::RenderWindow& window)
{
    // Оновлюємо позицію фону під камеру (центруємо по UI камері)
    // Але оскільки ми малюємо інвентар через uiView, позиції 200,50 правильні.
    
    window.draw(background);
    window.draw(titleText);

    if (inventoryFont.getInfo().family.empty()) return;

    sf::Text itemText;
    itemText.setFont(inventoryFont);
    itemText.setCharacterSize(20);

    float startX = background.getPosition().x + 40;
    float startY = background.getPosition().y + 60;

    if (items.empty())
    {
        itemText.setString("(Empty)");
        itemText.setFillColor(sf::Color(150, 150, 150));
        itemText.setPosition(startX, startY);
        window.draw(itemText);
    }
    else
    {
        for (size_t i = 0; i < items.size(); ++i)
        {
            std::string line = items[i].name;
            
            // Якщо це виділений предмет - додаємо стрілочку і колір
            if (static_cast<int>(i) == selectedIndex) {
                line = "> " + line + " <";
                itemText.setFillColor(sf::Color::Yellow);
            } else {
                itemText.setFillColor(sf::Color::White);
            }

            itemText.setString(line);
            itemText.setPosition(startX, startY + (i * 30));
            window.draw(itemText);
        }
    }
}