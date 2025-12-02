#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Player.hpp"
#include "Projectile.hpp"
#include "Enemy.hpp"

class Game
{
public:
    Game();
    void run();

private:
    void processEvents();
    void update();
    void render();
    
    void spawnEnemy(); 
    void resetGame(); 
    void loadLevel(int level);
    void handleStationCollision(const sf::Sprite& station);
    
    // --- ЗБЕРЕЖЕННЯ ПРОГРЕСУ ---
    void saveGame();
    void loadGame();
    // ----------------------------

    std::string wrapText(const std::string& str, float width, const sf::Font& font, int charSize);
    void startDialogue(const std::vector<std::string>& lines);
    void advanceDialogue();

    bool isDialogueActive;
    std::vector<std::string> dialogueQueue; 
    int currentDialogueIndex;

    sf::RectangleShape dialogueBox;
    sf::Text dialogueText;
    sf::Text dialogueHint;

    enum class GameState { Playing, InventoryOpen };

    GameState currentState;
    
    int currentLevel; 
    bool isFiring = false;   
    bool isGameOver = false; 
    bool isGameWon = false;
    
    bool canDock = false;

    sf::RenderWindow window;
    sf::View gameView; 
    sf::View uiView;   

    Player player;

    sf::Texture bgTexture;
    sf::Sprite bgSprite;

    sf::Texture stationTexture;
    sf::Sprite startStation;
    sf::Sprite endStation;

    sf::Texture blackHoleTexture;
    sf::Sprite blackHole;

    sf::Texture arrowTexture;
    sf::Sprite arrowSprite; 

    sf::Texture enemyTexture;
    std::vector<Enemy> enemies;
    sf::Clock spawnClock;

    std::vector<Projectile> bullets;
    sf::Clock shootClock;

    struct Explosion {
        sf::CircleShape shape;
        float duration;
    };
    std::vector<Explosion> explosions;

    sf::Font font;
    sf::Text uiText;     
    sf::Text missionText; 
    int score;
};