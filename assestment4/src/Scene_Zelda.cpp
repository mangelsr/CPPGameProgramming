#include "Scene_Zelda.h"

void Scene_Zelda::loadLevel(const std::string &filename)
{
    m_entityManager = EntityManager();

    // STUDENT TODO:
    // Load the level file and put all entities in the manager
    // Use the getPosition() function below to convert room-tile coords to game world coords

    spawnPlayer();
}

Vec2 Scene_Zelda::getPosition(int rx, int ry, int tx, int ty) const
{
    // STUDENT TODO:
    // Implement this function, which takes in the room (rx, ry) coordinate
    // as well as the tile (tx, ty) coordinate, and returns the Vec2 game world
    // position of the center of the entity.

    return Vec2(0, 0);
}

void Scene_Zelda::spawnPlayer()
{
    m_player = m_entityManager.addEntity("player");
    m_player->addComponent<CTransform>(Vec2(640, 480));
    m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandDown"), true);
    m_player->addComponent<CBoundingBox>(Vec2(48, 48), true, false);
    m_player->addComponent<CHealth>(7, 3);

    // STUDENT TODO:
    // Implement this function so that it uses the parameters input from the level file
    // Those parameters should be stored in the m_playerConfig variable
}

void Scene_Zelda::spawnSword(std::shared_ptr<Entity> entity)
{
    // STUDENT TODO:
    //
    // Implement the spawning of the sword, which:
    // - should be given the appropriate lifespan
    // - should spawn at the appropriate location based on player's facing direction
    // - be given a damage value of 1
    // - should play the "Slash" sound
}

void Scene_Zelda::update()
{
    m_entityManager.update();

    // STUDENT TODO:
    // Implement pause functionality (when 'P' key is pressed)

    sAI();
    sMovement();
    sStatus();
    sCollision();
    sAnimation();
    sCamera();

    m_currentFrame++;
}

void Scene_Zelda::sMovement()
{
    // STUDENT TODO:
    //
    // Implement all player movement functionality here based on
    // the player's input component variables
}

void Scene_Zelda::sDoAction(const Action &action)
{
    // STUDENT TODO:
    //
    // Implement all actions described for the game here
    // Only the setting of the player's input component variables should be set here
    // Do minimal logic in this function

    if (action.type() == "START")
    {
        if (action.name() == "PAUSE")
        {
            setPaused(!m_paused);
        }
        else if (action.name() == "QUIT")
        {
            onEnd();
        }
        else if (action.name() == "TOGGLE_FOLLOW")
        {
            m_follow = !m_follow;
        }
        else if (action.name() == "TOGGLE_TEXTURE")
        {
            m_drawTextures = !m_drawTextures;
        }
        else if (action.name() == "TOGGLE_COLLISION")
        {
            m_drawCollision = !m_drawCollision;
        }
    }
    else if (action.type() == "END")
    {
    }
}

void Scene_Zelda::sAI()
{
    // STUDENT TODO: Implement Enemy AI
    //
    // Implement Follow behavior
    // Implement Patrol behavior
}

void Scene_Zelda::sStatus()
{
    // STUDENT TODO:
    // Implement Lifespan Here
    // Implement Invincibility Frames here
}

void Scene_Zelda::sCollision()
{
    // STUDENT TODO:
    //
    // Implement entity - tile collisions
    // Implement player - enemy collisions with appropriate damage calculations
    // Implement Sword - NPC collisions
    // Implement black tile collisions / 'teleporting'
    // Implement entity - heart collisions and life gain logic
    //
    // You may want to use helper functions for these behaviors or this function will get long
}

void Scene_Zelda::sAnimation()
{
    // STUDENT TODO:
    //
    // Implement player facing direction animation
    // Implement sword animation based on player facing
    // The sword should move if the player changes direction mid swing
    // Implement destruction of entities with non-repeating finished animations
}

void Scene_Zelda::sCamera()
{
    // STUDENT TODO:
    //
    // Implement camera view logic

    // get the current view, which we will modify in the if-statement below
    sf::View view = m_game->window().getView();

    if (m_follow)
    {
        // calculate view for player follow camera
    }
    else
    {
        // calculate view for room-based camera
    }

    // then set the window view
    m_game->window().setView(view);
}