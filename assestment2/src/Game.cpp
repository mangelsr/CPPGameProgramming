#include <fstream>
#include <iostream>
#include <math.h>

#include "Game.h"

constexpr double PI = 3.14159265358979323846;

Game::Game(const std::string &config)
{
    init(config);
}

void Game::run()
{
    while (m_running)
    {
        if (!m_paused)
        {
            m_entities.update();

            sLifespan();
            sEnemySpawner();
            sMovement();
            sCollision();
        }

        sUserInput();
        sRender();

        if (m_paused)
        {
            // increment the current frame
            // may need to be moved when pause implemented
            m_currentFrame++;
        }
    }
}

void Game::init(const std::string &path)
{
    std::ifstream fileIn("config.txt");

    std::string rowIdentifier;
    int width, height, framerate, fullscreenMode;

    std::string fontName;
    int fontSize;
    sf::Color fontColor;

    while (fileIn >> rowIdentifier)
    {
        if (rowIdentifier == "Window")
        {
            // Window 1280 720 60 0
            fileIn >> width >> height >> framerate >> fullscreenMode;

            sf::VideoMode mode = sf::VideoMode(width, height);
            sf::Uint32 style = (fullscreenMode == 1) ? sf::Style::Fullscreen : sf::Style::Default;
            m_window.create(mode, "Assignment 2", style);
            m_window.setFramerateLimit(framerate);
        }
        else if (rowIdentifier == "Font")
        {
            // Font fonts/tech.ttf 24 255 255 255
            int fontColorR, fontColorG, fontColorB;
            fileIn >> fontName >> fontSize >> fontColorR >> fontColorG >> fontColorB;
            fontColor = sf::Color(fontColorR, fontColorG, fontColorB);

            if (!m_font.loadFromFile(fontName))
            {
                std::cerr << "Could not load font!";
                exit(-1);
            }
            m_text = sf::Text("Score: " + std::to_string(m_score), m_font, fontSize);
            m_text.setFillColor(fontColor);
        }
        else if (rowIdentifier == "Player")
        {
            // Player 32 32 5 5 5 5 255 0 0 4 8
            // Player SR CR S FR FG FB OR OG OB OT V
            fileIn >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S >>
                m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB >>
                m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB >> m_playerConfig.OT >>
                m_playerConfig.V;
        }
        else if (rowIdentifier == "Enemy")
        {
            // Enemy 32 32 3 3 255 255 255 2 3 8 90 60
            // Enemy SR CR SMIN SMAX OR OG OB OT VMIN VMAX L SP
            fileIn >> m_enemyConfig.SR >> m_enemyConfig.CR >>
                m_enemyConfig.SMIN >> m_enemyConfig.SMAX >>
                m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB >> m_enemyConfig.OT >>
                m_enemyConfig.VMIN >> m_enemyConfig.VMAX >>
                m_enemyConfig.L >> m_enemyConfig.SP;
        }
        else if (rowIdentifier == "Bullet")
        {
            // Bullet 10 10 20 255 255 255 255 255 2 20 90
            // Bullet SR CR S FR FG FB OR OG OB OT V L
            fileIn >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S >>
                m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB >>
                m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB >> m_bulletConfig.OT >>
                m_bulletConfig.V >> m_bulletConfig.L;
        }
    }

    spawnPlayer();
}

void Game::setPaused(bool paused)
{
    m_paused = paused;
}

// Spawn Entities
// respawn the player in the middle of the screen
void Game::spawnPlayer()
{
    auto entity = m_entities.addEntity("player");

    auto wSize = m_window.getSize();
    entity->cTransform = std::make_shared<CTransform>(
        Vec2(wSize.x / 2, wSize.y / 2), Vec2(0, 0), 0.0f);

    entity->cShape = std::make_shared<CShape>(
        m_playerConfig.SR, m_playerConfig.V,
        sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB),
        sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB),
        m_playerConfig.OT);

    entity->cInput = std::make_shared<CInput>();

    entity->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);

    m_player = entity;
}

// spawn an enemy at a random position
void Game::spawnEnemy()
{
    auto entity = m_entities.addEntity("enemy");

    auto wSize = m_window.getSize();
    float posX = random(m_enemyConfig.SR, wSize.x - m_enemyConfig.SR);
    float posY = random(m_enemyConfig.SR, wSize.y - m_enemyConfig.SR);
    Vec2 entityPosition = Vec2(posX, posY);

    int vertexCount = random(m_enemyConfig.VMIN, m_enemyConfig.VMAX);

    int speed = random(m_enemyConfig.SMIN, m_enemyConfig.SMAX);
    int angle = random(0, 360);

    float velX = speed * cosf(angle);
    float velY = speed * sinf(angle);
    Vec2 entityVelocity = Vec2(velX, velY);

    sf::Color fillColor = sf::Color(random(0, 255), random(0, 255), random(0, 255));

    entity->cShape = std::make_shared<CShape>(
        m_enemyConfig.SR, vertexCount,
        fillColor,
        sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB),
        m_enemyConfig.OT);

    entity->cTransform = std::make_shared<CTransform>(entityPosition, entityVelocity, 0);

    entity->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR);

    entity->cScore = std::make_shared<CScore>(vertexCount * 100);

    // record when the most recent enemy was spawned
    m_lastEnemySpawnTime = m_currentFrame;
}

// spawns the small enemies when a big one (input entity e) explodes
void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{
    sf::CircleShape &shape = e->cShape->circle;

    int vertexCount = shape.getPointCount();
    float shapeRadius = shape.getRadius() / 2;
    float outlineThickness = shape.getOutlineThickness();
    sf::Color fillColor = shape.getFillColor();
    sf::Color outlineColor = shape.getOutlineColor();

    Vec2 position = Vec2(e->cTransform->pos.x, e->cTransform->pos.y);

    float collisionRadius = e->cCollision->radius / 2;

    int score = e->cScore->score * 2;

    float speed = e->cTransform->velocity.module();
    float angleStep = (2 * PI) / vertexCount; // Use rads

    for (int i = 0; i < vertexCount; i++)
    {
        float direction = angleStep * i;

        float velX = speed * cosf(direction);
        float velY = speed * sinf(direction);
        Vec2 velocity = Vec2(velX, velY);

        auto smallEnemy = m_entities.addEntity("small enemy");

        smallEnemy->cShape = std::make_shared<CShape>(
            shapeRadius, vertexCount,
            fillColor, outlineColor, outlineThickness);

        smallEnemy->cTransform = std::make_shared<CTransform>(position, velocity, 0);

        smallEnemy->cCollision = std::make_shared<CCollision>(shapeRadius);

        smallEnemy->cScore = std::make_shared<CScore>(score);

        smallEnemy->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);
    }
}

// spawns a bullet from a given entity to a target location
void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2 &target)
{
    auto bulletEntity = m_entities.addEntity("bullet");

    Vec2 entityPos = entity->cTransform->pos;
    Vec2 initialPosition = Vec2(entityPos.x, entityPos.y);
    Vec2 direction = target - initialPosition;
    Vec2 velocity = direction.normalize() * m_bulletConfig.S;

    bulletEntity->cTransform = std::make_shared<CTransform>(initialPosition, velocity, 0.0f);

    bulletEntity->cShape = std::make_shared<CShape>(
        m_bulletConfig.SR, m_bulletConfig.V,
        sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB),
        sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB),
        m_bulletConfig.OT);

    bulletEntity->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);

    bulletEntity->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
    // TODO: implement your own special weapon
}

// Systems
void Game::sMovement()
{
    m_player->cTransform->velocity = {0, 0};

    if (m_player->cInput->up)
    {
        m_player->cTransform->velocity.y = -m_playerConfig.S;
    }

    if (m_player->cInput->down)
    {
        m_player->cTransform->velocity.y = m_playerConfig.S;
    }

    if (m_player->cInput->left)
    {
        m_player->cTransform->velocity.x = -m_playerConfig.S;
    }

    if (m_player->cInput->right)
    {
        m_player->cTransform->velocity.x = m_playerConfig.S;
    }

    for (auto &e : m_entities.getEntities())
    {
        e->cTransform->pos.x += e->cTransform->velocity.x;
        e->cTransform->pos.y += e->cTransform->velocity.y;
    }
}

void Game::sUserInput()
{
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        // this event triggers when the window is closed
        if (event.type == sf::Event::Closed)
        {
            m_running = false;
        }

        // this event is triggered when a key is pressed
        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::W:
                m_player->cInput->up = true;
                break;
            case sf::Keyboard::A:
                m_player->cInput->left = true;
                break;
            case sf::Keyboard::S:
                m_player->cInput->down = true;
                break;
            case sf::Keyboard::D:
                m_player->cInput->right = true;
                break;
            case sf::Keyboard::P:
                setPaused(!m_paused);
                break;
            default:
                break;
            }
        }

        // this event is triggered when a key is released
        if (event.type == sf::Event::KeyReleased)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::W:
                m_player->cInput->up = false;
                break;
            case sf::Keyboard::A:
                m_player->cInput->left = false;
                break;
            case sf::Keyboard::S:
                m_player->cInput->down = false;
                break;
            case sf::Keyboard::D:
                m_player->cInput->right = false;
                break;
            default:
                break;
            }
        }

        if (!m_paused && event.type == sf::Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                auto mouseEvent = event.mouseButton;
                spawnBullet(m_player, Vec2(mouseEvent.x, mouseEvent.y));
            }

            if (event.mouseButton.button == sf::Mouse::Right)
            {
                spawnSpecialWeapon(m_player);
            }
        }
    }
}

void Game::sLifespan()
{
    for (auto &entity : m_entities.getEntities())
    {
        auto &lifespan = entity->cLifespan;

        if (lifespan == nullptr)
            continue;

        if (lifespan->remaining > 0)
        {
            lifespan->remaining--;

            auto &circle = entity->cShape->circle;
            int newAlpha = lifespan->remaining * 255 / lifespan->total;

            auto currentFillColor = circle.getFillColor();
            currentFillColor.a = newAlpha;

            auto currentOutlineColor = circle.getOutlineColor();
            currentOutlineColor.a = newAlpha;

            circle.setFillColor(currentFillColor);
            circle.setOutlineColor(currentOutlineColor);
        }
        else
        {
            entity->destroy();
        }
    }
}

void Game::sRender()
{
    m_window.clear();

    for (auto &e : m_entities.getEntities())
    {
        e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);

        e->cTransform->angle += 1.0f;
        e->cShape->circle.setRotation(e->cTransform->angle);

        m_window.draw(e->cShape->circle);
    }

    m_text.setString("Score: " + std::to_string(m_score));
    m_window.draw(m_text);

    m_window.display();
}

void Game::sEnemySpawner()
{
    if (m_currentFrame - m_lastEnemySpawnTime == m_enemyConfig.SP)
    {
        spawnEnemy();
    }
}

void Game::sCollision()
{
    auto &enemies = m_entities.getEntities("enemy");
    auto &smallEnemies = m_entities.getEntities("small enemy");

    auto allEnemies = EntityVec();
    allEnemies.reserve(enemies.size() + smallEnemies.size());
    allEnemies.insert(allEnemies.end(), enemies.begin(), enemies.end());
    allEnemies.insert(allEnemies.end(), smallEnemies.begin(), smallEnemies.end());

    auto bullets = m_entities.getEntities("bullet");

    for (auto &bullet : bullets)
    {
        if (bullet->isActive())
        {
            for (auto &enemy : allEnemies)
            {
                if (enemy->isActive())
                {
                    if (checkCollision(bullet, enemy))
                    {
                        m_score += enemy->cScore->score;
                        enemy->destroy();
                        bullet->destroy();
                        if (enemy->tag() == "enemy")
                        {
                            spawnSmallEnemies(enemy);
                        }
                    }
                }
            }
        }
    }

    for (auto &enemy : allEnemies)
    {
        if (enemy->isActive())
        {
            if (checkCollision(enemy, m_player))
            {
                auto wSize = m_window.getSize();
                Vec2 &playerPostition = m_player->cTransform->pos;
                playerPostition.x = wSize.x / 2;
                playerPostition.y = wSize.y / 2;
            }
        }
    }
}

int Game::random(int min, int max)
{
    return min + (rand() % (1 + max - min));
}

bool Game::checkCollision(std::shared_ptr<Entity> entity1, std::shared_ptr<Entity> entity2)
{
    auto e1Center = entity1->cShape->circle.getPosition();
    auto e2center = entity2->cShape->circle.getPosition();

    float dist = Vec2(e1Center.x, e1Center.y)
                     .dist(Vec2(e2center.x, e2center.y));

    float e1Radius = entity1->cCollision->radius;
    float e2Radius = entity2->cCollision->radius;

    return dist < e1Radius + e2Radius;
}