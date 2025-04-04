#pragma once

#include <map>
#include <memory>

#include "Scene.h"
#include "EntityManager.h"
#include "GameEngine.h"

class Scene_Zelda : public Scene
{
    struct PlayerConfig
    {
        float X, Y, CX, CY, SPEED, HEALTH;
        std::string WEAPON;
    };

protected:
    std::shared_ptr<Entity> m_player;
    std::string m_levelPath;
    PlayerConfig m_playerConfig;

    bool m_drawTextures = true;
    bool m_drawCollision = false;
    bool m_follow = false;

    void init(const std::string &levelPath);
    void loadLevel(const std::string &filename);

    void spawnPlayer();
    void spawnSword(std::shared_ptr<Entity> entity);
    Vec2 getPosition(int rx, int ry, int tx, int ty) const;

    void onEnd();
    void update();

    void sAI();
    void sAnimation();
    void sCamera();
    void sCollision();
    void sDoAction(const Action &action);
    void sMovement();
    void sRender();
    void sStatus();

public:
    Scene_Zelda(GameEngine *gameEngine, const std::string &levelPath);
};