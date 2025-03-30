#pragma once

#include <map>
#include <memory>
#include <string>

#include <SFML/Graphics.hpp>

#include "Action.h"
#include "Components.h"
#include "EntityManager.h"
#include "GameEngine.h"
#include "Scene.h"
#include "Vec2.h"

class Scene_Play : public Scene
{
    struct PlayerConfig
    {
        float X, Y, CX, CY, SPEED, MAXSPEED, JUMP, GRAVITY;
        std::string WEAPON;
    };

protected:
    std::shared_ptr<Entity> m_player;
    std::string m_levelPath;
    PlayerConfig m_playerConfig;
    bool m_drawTextures = true;
    bool m_drawCollision = false;
    bool m_drawGrid = false;
    const Vec2 m_gridSize = {64, 64};
    sf::Text m_gridText;

    Vec2 m_mousePos;
    sf::CircleShape m_mouseShape;

    void init(const std::string &levelPath);
    Vec2 gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);
    void loadLevel(const std::string &filename);
    void spawnPlayer();
    void spawnBullet(std::shared_ptr<Entity> entity);
    void update();

    Vec2 windowToWorld(const Vec2 &windowPos) const;

    void sDragAndDrop();
    void sMovement();
    void sState();
    void sLifespan();
    void sCollision();
    void sDoAction(const Action &action);
    void sAnimation();
    void sRender();

    void onEnd();

public:
    Scene_Play(GameEngine *gameEngine, const std::string &levelPath);
};