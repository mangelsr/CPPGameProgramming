#pragma once

#include <SFML/Graphics.hpp>

#include "Entity.h"
#include "EntityManager.h"

struct PlayerConfig
{
    int SR, CR, FR, FG, FB, OR, OG, OB, OT, V;
    float S;
};
struct EnemyConfig
{
    int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI;
    float SMIN, SMAX;
};
struct BulletConfig
{
    int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L;
    float S;
};

class Game
{
    sf::RenderWindow m_window; // the window we will draw to
    EntityManager m_entities;  // vector of entities to maintain
    sf::Font m_font;           // the font we will use to draw
    sf::Text m_text;           // the score text to be drawn to the screen
    PlayerConfig m_playerConfig;
    EnemyConfig m_enemyConfig;
    BulletConfig m_bulletConfig;
    int m_score = 0;
    int m_currentFrame = 0;
    int m_lastEnemySpawnTime = 0;
    bool m_paused = false; // whether we update game logic
    bool m_running = true; // whether the game is running

    std::shared_ptr<Entity> m_player; // the player entity

    void init(const std::string &config); // Initialize the game with a config file
    void setPaused(bool paused);          // Pause the game

    // Systems
    void sMovement();     // System: Entity position/movement update
    void sUserInput();    // System: User input handling
    void sLifespan();     // System: Lifespan management
    void sRender();       // System: Rendering/drawing
    void sEnemySpawner(); // System: Spawn enemies
    void sCollision();    // System: Collisions detection and resolution

    // Spawn entities
    void spawnPlayer();
    void spawnEnemy();
    void spawnSmallEnemies(std::shared_ptr<Entity> entity);
    void spawnBullet(std::shared_ptr<Entity> entity, const Vec2 &mousePos);
    void spawnSpecialWeapon(std::shared_ptr<Entity> entity);

public:
    Game(const std::string &config); // constructor, takes the game config
    void run();
};