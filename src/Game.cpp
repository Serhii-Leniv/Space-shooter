#include "Game.hpp"
#include "Item.hpp"
#include <iostream>
#include <cmath>
#include <sstream> 
#include <fstream> // Для збереження файлів

const float PI = 3.14159265f;

Game::Game()
    : window(sf::VideoMode(800, 600), "Space Quest: Into the Black Hole"),
      player(400.0f, 500.0f),
      currentState(GameState::Playing),
      score(0),
      currentLevel(1),
      isDialogueActive(false),
      canDock(false)
{
    window.setFramerateLimit(60);

    gameView.setSize(1200, 900);  
    gameView.setCenter(400, 300);
    uiView.setSize(800, 600);
    uiView.setCenter(400, 300);

    if (!font.loadFromFile("res/fonts/pixel_font.ttf")) std::cerr << "Font error" << std::endl;
    if (!enemyTexture.loadFromFile("res/textures/enemy.png")) std::cerr << "Enemy tex error" << std::endl;
    
    // --- ДІАЛОГОВЕ ВІКНО ---
    dialogueBox.setSize(sf::Vector2f(700, 150));
    dialogueBox.setFillColor(sf::Color(0, 0, 0, 200));
    dialogueBox.setOutlineColor(sf::Color::Cyan);
    dialogueBox.setOutlineThickness(2.0f);
    dialogueBox.setPosition(50, 400);

    dialogueText.setFont(font);
    dialogueText.setCharacterSize(20);
    dialogueText.setFillColor(sf::Color::White);
    dialogueText.setPosition(70, 420);
    dialogueText.setLineSpacing(1.5f); 

    dialogueHint.setFont(font);
    dialogueHint.setCharacterSize(14);
    dialogueHint.setFillColor(sf::Color::Yellow);
    dialogueHint.setString("Press ENTER to continue...");
    dialogueHint.setPosition(550, 520);
    // -----------------------

    if (stationTexture.loadFromFile("res/textures/station.png")) {
        startStation.setTexture(stationTexture);
        startStation.setOrigin(stationTexture.getSize().x / 2, stationTexture.getSize().y / 2);
        startStation.setScale(2.0f, 2.0f);
        
        endStation.setTexture(stationTexture);
        endStation.setOrigin(stationTexture.getSize().x / 2, stationTexture.getSize().y / 2);
        endStation.setScale(2.0f, 2.0f);
    }

    // --- ЧОРНА ДІРА (Картинка) ---
    if (blackHoleTexture.loadFromFile("res/textures/blackhole.png")) {
        blackHole.setTexture(blackHoleTexture);
        // Центруємо для зручного обертання або позиціонування
        blackHole.setOrigin(blackHoleTexture.getSize().x / 2, blackHoleTexture.getSize().y / 2);
        blackHole.setScale(3.0f, 3.0f); // Масштаб 3x
    } else {
        std::cerr << "ERROR: res/textures/blackhole.png not found! Check filename." << std::endl;
    }

    if (arrowTexture.loadFromFile("res/textures/arrow.png")) {
        arrowSprite.setTexture(arrowTexture);
        arrowSprite.setOrigin(arrowTexture.getSize().x / 2, arrowTexture.getSize().y / 2);
        arrowSprite.setScale(0.1f, 0.1f);
    }

    player.inventory.setFont(font);
    
    uiText.setFont(font);
    uiText.setCharacterSize(24);
    uiText.setPosition(10, 40);
    uiText.setFillColor(sf::Color::White);

    missionText.setFont(font);
    missionText.setCharacterSize(18); 
    missionText.setPosition(10, 5);
    missionText.setFillColor(sf::Color::Yellow);
    
    player.inventory.addItem(Item("Energy Cell", ItemType::EnergyCell));
    
    loadLevel(1);
}

// --- ЗБЕРЕЖЕННЯ ---
void Game::saveGame() {
    std::ofstream file("savegame.txt");
    if (file.is_open()) {
        file << currentLevel << " " << score;
        file.close();
        uiText.setString("GAME SAVED!");
        std::cout << "Game Saved!" << std::endl;
    }
}

// --- ЗАВАНТАЖЕННЯ ---
void Game::loadGame() {
    std::ifstream file("savegame.txt");
    if (file.is_open()) {
        int lvl, scr;
        file >> lvl >> scr;
        file.close();
        
        score = scr;
        loadLevel(lvl); // Завантажуємо збережений рівень
        uiText.setString("GAME LOADED! Score: " + std::to_string(score));
        std::cout << "Game Loaded!" << std::endl;
    }
}

std::string Game::wrapText(const std::string& str, float width, const sf::Font& font, int charSize) {
    std::string wrappedStr;
    std::string currentLine;
    std::stringstream ss(str);
    std::string word;
    
    while (ss >> word) {
        std::string testLine = currentLine + (currentLine.empty() ? "" : " ") + word;
        sf::Text text(testLine, font, charSize);
        if (text.getLocalBounds().width > width) {
            wrappedStr += currentLine + "\n";
            currentLine = word;
        } else {
            currentLine = testLine;
        }
    }
    wrappedStr += currentLine;
    return wrappedStr;
}

void Game::startDialogue(const std::vector<std::string>& lines) {
    dialogueQueue = lines;
    currentDialogueIndex = 0;
    if (!dialogueQueue.empty()) {
        isDialogueActive = true;
        std::string wrapped = wrapText(dialogueQueue[0], 660.0f, font, 20);
        dialogueText.setString(wrapped);
    }
}

void Game::advanceDialogue() {
    currentDialogueIndex++;
    if (currentDialogueIndex < dialogueQueue.size()) {
        std::string wrapped = wrapText(dialogueQueue[currentDialogueIndex], 660.0f, font, 20);
        dialogueText.setString(wrapped);
    } else {
        // Діалог закінчився
        isDialogueActive = false;
        
        // Оновлюємо місію або стан гри після діалогу
        if (currentLevel == 1) {
             missionText.setString("TASK: Reach the Black Hole! (Follow Arrow)");
        }
        
        // --- ФІНАЛ ГРИ ---
        // Якщо це був діалог на 2-му рівні біля станції (canDock), то це перемога
        if (currentLevel == 2 && canDock) {
            isGameWon = true;
            uiText.setString("VICTORY!\nThe Galaxy is safe.\n\nPress 'R' to Restart");
            uiText.setFillColor(sf::Color::Green);
            uiText.setCharacterSize(30);
            uiText.setPosition(250, 250); 
        }
    }
}

void Game::loadLevel(int level) {
    currentLevel = level;
    enemies.clear();
    bullets.clear();
    explosions.clear();
    canDock = false;
    isGameWon = false;
    
    if (level == 1) {
        if (bgTexture.loadFromFile("res/textures/background.png")) {
            bgTexture.setRepeated(true);
            bgSprite.setTexture(bgTexture);
            bgSprite.setTextureRect(sf::IntRect(-20000, -20000, 40000, 40000));
            bgSprite.setPosition(-20000, -20000);
        }
        
        // --- БЕЗПЕЧНИЙ СПАВН (Рівень 1) ---
        // Станція знизу, гравець вище
        startStation.setPosition(400, 800); 
        player.shape.setPosition(400, 600); 
        
        // --- ЗМІНА ДИСТАНЦІЇ ---
        // Відсунув чорну діру далі (-8000), щоб політ тривав довше
        blackHole.setPosition(400, -8000); 
        // ------------------------
        
        std::vector<std::string> intro;
        intro.push_back("COMMANDER: Pilot, the station is compromised!");
        intro.push_back("COMMANDER: Your only chance is the Black Hole to the North.");
        intro.push_back("COMMANDER: It acts as a warp gate to Sector Alpha.");
        intro.push_back("COMMANDER: Beware of enemies on your radar. Good luck.");
        startDialogue(intro);
        
        missionText.setString("TASK: Listen to Commander...");
    } 
    else if (level == 2) {
        // Пробуємо завантажити інший фон, інакше старий
        if (!bgTexture.loadFromFile("res/textures/background2.png")) {
             bgTexture.loadFromFile("res/textures/background.png"); 
        }
        bgTexture.setRepeated(true);
        bgSprite.setTexture(bgTexture);
        bgSprite.setTextureRect(sf::IntRect(-20000, -20000, 40000, 40000));
        bgSprite.setPosition(-20000, -20000);
        
        player.shape.setPosition(400, 500);
        endStation.setPosition(400, -5000); // Теж трохи ближче
        
        std::vector<std::string> lvl2;
        lvl2.push_back("SYSTEM: Warp successful.");
        lvl2.push_back("SYSTEM: Entering Unknown Sector.");
        lvl2.push_back("SYSTEM: Alpha Station signal detected ahead.");
        startDialogue(lvl2);

        missionText.setString("TASK: Find the Alpha Station!");
    }
}

void Game::run()
{
    while (window.isOpen())
    {
        processEvents();
        update();
        render();
    }
}

void Game::processEvents()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed) window.close();

        if (isDialogueActive) {
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                advanceDialogue();
            }
            return; 
        }

        if (currentState == GameState::Playing && !isGameOver && !isGameWon) {
            player.handleEvent(event);
        }

        if (event.type == sf::Event::KeyPressed)
        {
            if (currentState == GameState::InventoryOpen) 
            {
                if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up) player.inventory.selectPrev();
                if (event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down) player.inventory.selectNext();
                if (event.key.code == sf::Keyboard::Enter) {
                    if (player.inventory.hasItems()) {
                        Item item = player.inventory.getItemAndRemove();
                        if (item.type == ItemType::EnergyCell) {
                            sf::Vector2f pos = player.getPosition();
                            Explosion ex;
                            ex.shape.setRadius(10.f);
                            ex.shape.setPosition(pos);
                            ex.shape.setFillColor(sf::Color(255, 100, 0, 150));
                            ex.duration = 2.0f;
                            explosions.push_back(ex);
                            currentState = GameState::Playing;
                        }
                    }
                }
            }

            if (event.key.code == sf::Keyboard::I) {
                if (currentState == GameState::Playing) currentState = GameState::InventoryOpen;
                else currentState = GameState::Playing;
            }

            if (event.key.code == sf::Keyboard::R) {
                if (isGameOver || isGameWon) resetGame();
            }

            if (event.key.code == sf::Keyboard::Space) {
                if (currentState == GameState::Playing && !isGameOver && !isGameWon) isFiring = true;
            }
            
            // --- ЗБЕРЕЖЕННЯ ---
            if (event.key.code == sf::Keyboard::F5) saveGame();
            if (event.key.code == sf::Keyboard::F6) loadGame();

            // --- ВЗАЄМОДІЯ (E) ---
            if (event.key.code == sf::Keyboard::E) {
                if (canDock && currentLevel == 2) {
                    // Замість миттєвої перемоги, запускаємо фінальний діалог
                    std::vector<std::string> finale;
                    finale.push_back("ALPHA COMMANDER: Identify yourself!");
                    finale.push_back("PLAYER: I am the last survivor from Delta Outpost.");
                    finale.push_back("ALPHA COMMANDER: Open the docking bay doors! Get them in!");
                    finale.push_back("SYSTEM: Mission Accomplished.");
                    startDialogue(finale);
                    // Перемога настане в advanceDialogue, коли діалог закінчиться
                }
            }
        }
        
        if (event.type == sf::Event::KeyReleased) {
             if (event.key.code == sf::Keyboard::Space) isFiring = false;
             if (currentState == GameState::Playing) player.handleEvent(event);
        }
    }
}

void Game::resetGame() {
    score = 0;
    isGameOver = false;
    isGameWon = false;
    isFiring = false;
    canDock = false;
    currentState = GameState::Playing;
    
    uiText.setString("Score: 0");
    uiText.setCharacterSize(24);
    uiText.setFillColor(sf::Color::White);
    uiText.setPosition(10, 40);
    
    loadLevel(1); 
}

void Game::handleStationCollision(const sf::Sprite& station) {
    sf::FloatRect pBounds = player.getBounds();
    sf::FloatRect sBounds = station.getGlobalBounds();
    
    sBounds.left += 20; sBounds.top += 20;
    sBounds.width -= 40; sBounds.height -= 40;

    if (pBounds.intersects(sBounds)) {
        sf::Vector2f pCenter = player.getPosition();
        sf::Vector2f sCenter = station.getPosition();
        float dx = pCenter.x - sCenter.x;
        float dy = pCenter.y - sCenter.y;
        float dist = std::sqrt(dx*dx + dy*dy);
        if (dist > 0) {
            float pushForce = 5.0f; 
            player.shape.move((dx/dist) * pushForce, (dy/dist) * pushForce);
        }
    }
}

void Game::update()
{
    if (isDialogueActive) return; 
    if (currentState == GameState::InventoryOpen) return;
    if (isGameOver || isGameWon) return;

    player.update(); 
    
    if (currentLevel == 1) handleStationCollision(startStation);
    else handleStationCollision(endStation);

    sf::Sprite* currentStation = (currentLevel == 1) ? &startStation : &endStation;
    sf::FloatRect sBounds = currentStation->getGlobalBounds();
    sBounds.left += 10; sBounds.top += 10; sBounds.width -= 20; sBounds.height -= 20;

    for (size_t i = 0; i < enemies.size();) {
        enemies[i].update(player.getPosition());
        
        if (enemies[i].getBounds().intersects(sBounds)) {
            sf::Vector2f ePos = enemies[i].getPosition();
            sf::Vector2f sPos = currentStation->getPosition();
            float dx = ePos.x - sPos.x;
            float dy = ePos.y - sPos.y;
            float dist = std::sqrt(dx*dx + dy*dy);
            if (dist > 0) {
                enemies[i].move((dx/dist) * 5.0f, (dy/dist) * 5.0f);
            }
        }

        if (enemies[i].getBounds().intersects(player.getBounds())) {
            isGameOver = true;
            uiText.setString("GAME OVER\nPress 'R' to Restart");
            uiText.setFillColor(sf::Color::Red);
            uiText.setPosition(250, 250);
            return; 
        }

        bool enemyDied = false;
        
        for (size_t k = 0; k < bullets.size();) {
            if (bullets[k].getBounds().intersects(enemies[i].getBounds())) {
                bullets.erase(bullets.begin() + k);
                if (enemies[i].takeDamage(1)) {
                    enemyDied = true;
                    score += 10;
                    if (rand() % 5 == 0) player.inventory.addItem(Item("Energy Cell", ItemType::EnergyCell));
                }
                break; 
            } else ++k;
        }
        
        if (!enemyDied) {
            for (auto& ex : explosions) {
                sf::Vector2f ePos = enemies[i].getPosition();
                sf::Vector2f exPos = ex.shape.getPosition();
                float dist = std::sqrt(std::pow(ePos.x - exPos.x, 2) + std::pow(ePos.y - exPos.y, 2));
                if (dist < ex.shape.getRadius()) {
                    enemies[i].takeDamage(100);
                    enemyDied = true;
                    score += 5;
                    break;
                }
            }
        }

        if (enemyDied) enemies.erase(enemies.begin() + i);
        else ++i;
    }

    sf::Vector2f targetPos;
    if (currentLevel == 1) targetPos = blackHole.getPosition();
    else targetPos = endStation.getPosition();
    
    sf::Vector2f playerPos = player.getPosition();
    float dx = targetPos.x - playerPos.x;
    float dy = targetPos.y - playerPos.y;
    float radians = std::atan2(dy, dx);
    float angle = radians * 180 / PI;
    
    arrowSprite.setRotation(angle);
    float radius = 250.0f;
    float arrowX = 400.0f + std::cos(radians) * radius;
    float arrowY = 300.0f + std::sin(radians) * radius;
    arrowSprite.setPosition(arrowX, arrowY);

    if (currentLevel == 1) {
        // Перехід тільки через чорну діру (intersects)
        if (player.getBounds().intersects(blackHole.getGlobalBounds())) {
            loadLevel(2);
            return; 
        }
    } else if (currentLevel == 2) {
        sf::Vector2f sPos = endStation.getPosition();
        float dist = std::sqrt(std::pow(playerPos.x - sPos.x, 2) + std::pow(playerPos.y - sPos.y, 2));
        
        // --- ЗБІЛЬШЕНА ЗОНА ДЛЯ СТИКОВКИ (500px) ---
        if (dist < 500.0f) {
            canDock = true;
            missionText.setString("Press 'E' to Enter Station");
            missionText.setFillColor(sf::Color::Green);
        } else {
            canDock = false;
            missionText.setString("TASK: Find the Alpha Station!");
            missionText.setFillColor(sf::Color::Yellow);
        }
    }

    if (isFiring) {
        if (shootClock.getElapsedTime().asSeconds() > 0.2f) {
             bullets.emplace_back(playerPos.x, playerPos.y - 30, 0.0f, -1.0f);
             shootClock.restart();
        }
    }

    sf::Vector2f vPos = gameView.getCenter();
    sf::Vector2f newCenter = vPos + (playerPos - vPos) * 0.1f;
    gameView.setCenter(newCenter);

    for (size_t i = 0; i < bullets.size();) {
        bullets[i].update();
        if (std::abs(bullets[i].shape.getPosition().y - gameView.getCenter().y) > 1000) {
             bullets.erase(bullets.begin() + i);
        } else ++i;
    }
    
    for (size_t i = 0; i < explosions.size();) {
        explosions[i].duration -= 0.016f; 
        float r = explosions[i].shape.getRadius();
        explosions[i].shape.setRadius(r + 5.0f);
        explosions[i].shape.setOrigin(r + 5.0f, r + 5.0f);
        if (explosions[i].duration <= 0) explosions.erase(explosions.begin() + i);
        else ++i;
    }

    if (spawnClock.getElapsedTime().asSeconds() > 1.0f) {
        spawnEnemy();
        spawnClock.restart();
    }

    std::string currentText = uiText.getString();
    if (currentText.find("SAVED") == std::string::npos && currentText.find("LOADED") == std::string::npos) {
         uiText.setString("Score: " + std::to_string(score));
    }
}

void Game::spawnEnemy()
{
    sf::Vector2f center = gameView.getCenter();
    float x = center.x + (rand() % 1300 - 650); 
    float y = center.y - 600; 
    
    sf::Sprite* station = (currentLevel == 1) ? &startStation : &endStation;
    sf::Vector2f sPos = station->getPosition();
    float dist = std::sqrt(std::pow(x - sPos.x, 2) + std::pow(y - sPos.y, 2));
    
    if (dist < 300.0f) {
        return; 
    }

    int roll = rand() % 100;
    EnemyType type = EnemyType::Small;
    if (roll > 60) type = EnemyType::Medium;
    if (roll > 90) type = EnemyType::Tank;

    enemies.emplace_back(x, y, type, enemyTexture);
}

void Game::render()
{
    window.clear(sf::Color(5, 5, 20));
    
    window.setView(gameView);
    if (bgSprite.getTexture() != nullptr) window.draw(bgSprite);
    
    if (currentLevel == 1) {
        window.draw(startStation);
        window.draw(blackHole);
    } else {
        window.draw(endStation);
    }

    for (auto& ex : explosions) window.draw(ex.shape);
    for (auto& b : bullets) b.draw(window);
    for (auto& e : enemies) e.draw(window);
    player.draw(window);
    
    window.setView(uiView);
    window.draw(uiText);
    window.draw(missionText);
    
    if (!isGameOver && !isGameWon) {
        window.draw(arrowSprite);
    }

    if (isDialogueActive) {
        window.draw(dialogueBox);
        window.draw(dialogueText);
        window.draw(dialogueHint);
    }

    if (currentState == GameState::InventoryOpen) {
        player.inventory.draw(window);
    }

    window.display();
}