#pragma once
#include <string>

enum class ItemType {
    ScoreBonus, // Просто очки
    EnergyCell  // Бомба/Вибух
};

struct Item
{
    std::string name;
    ItemType type;

    Item(const std::string& name, ItemType type = ItemType::ScoreBonus) 
        : name(name), type(type) {}
};