#include "Scene_Zelda.h"

Scene_Zelda::Scene_Zelda(GameEngine *gameEngine, const std::string &levelPath)
    : Scene(gameEngine), m_levelPath(levelPath)
{
    init(m_levelPath);
}

void Scene_Zelda::init(const std::string &levelPath)
{
    loadLevel(levelPath);

    // STUDENT TODO:
    // Register the actions required to play the game

    registerAction(sf::Keyboard::Escape, "QUIT");
    registerAction(sf::Keyboard::P, "PAUSE");
    registerAction(sf::Keyboard::Y, "TOGGLE_FOLLOW");    // Toggle Follow/Room Camera
    registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");   // Toggle drawing (T)extures
    registerAction(sf::Keyboard::C, "TOGGLE_COLLISION"); // Toggle drawing (C)ollision Boxes
}

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

void Scene_Zelda::sRender()
{
    // Color the background darker when paused
    if (!m_paused)
    {
        m_game->window().clear(sf::Color(100, 100, 255));
    }
    else
    {
        m_game->window().clear(sf::Color(50, 50, 150));
    }

    // Draw all Entity textures/animations
    if (m_drawTextures)
    {
        for (const std::shared_ptr<Entity> &e : m_entityManager.getEntities()) // Use a reference for efficiency
        {
            CTransform &transform = e->getComponent<CTransform>(); // Use a reference for efficiency

            if (e->hasComponent<CAnimation>())
            {
                Animation &animation = e->getComponent<CAnimation>().animation; // Use a reference!
                sf::Sprite &sprite = animation.getSprite();                     // Get a reference to the sprite

                sprite.setRotation(transform.angle);
                sprite.setPosition(transform.pos.x, transform.pos.y);
                sprite.setScale(transform.scale.x, transform.scale.y);

                m_game->window().draw(sprite);
            }
        }
    }

    // Draw all Entity collision bounding boxes with a RectangleShape
    if (m_drawCollision)
    {
        for (const std::shared_ptr<Entity> &e : m_entityManager.getEntities())
        {
            if (e->hasComponent<CBoundingBox>())
            {
                CBoundingBox &box = e->getComponent<CBoundingBox>();
                CTransform &transform = e->getComponent<CTransform>();

                sf::RectangleShape rect;
                rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1)); // Subtract 1 for visibility
                rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
                rect.setPosition(transform.pos.x, transform.pos.y);
                rect.setFillColor(sf::Color(0, 0, 0, 0));

                if (box.blockMove && box.blockVision)
                {
                    rect.setOutlineColor(sf::Color::Black);
                }
                else if (box.blockMove)
                {
                    rect.setOutlineColor(sf::Color::Blue);
                }
                else if (box.blockVision)
                {
                    rect.setOutlineColor(sf::Color::Red);
                }
                else
                {
                    rect.setOutlineColor(sf::Color::White);
                }

                rect.setOutlineThickness(1);
                m_game->window().draw(rect);
            }

            Vec2 playerCenter = m_player->getComponent<CTransform>().pos;

            if (e->hasComponent<CFollowPlayer>())
            {
                Vec2 enemyCenter = e->getComponent<CTransform>().pos;

                sf::Vertex line[] = {
                    sf::Vertex(sf::Vector2f(playerCenter.x, playerCenter.y), sf::Color::Black),
                    sf::Vertex(sf::Vector2f(enemyCenter.x, enemyCenter.y), sf::Color::Black)};
                m_game->window().draw(line, 2, sf::Lines);
            }
            else if (e->hasComponent<CPatrol>())
            {
                CPatrol &patrol = e->getComponent<CPatrol>();

                for (const Vec2 &point : patrol.positions)
                {
                    sf::CircleShape waypoint(3.f);
                    waypoint.setFillColor(sf::Color::Black);
                    waypoint.setOutlineThickness(1.f);
                    waypoint.setOrigin(3.f, 3.f);
                    waypoint.setPosition(point.x, point.y);
                    m_game->window().draw(waypoint);
                }
            }
        }
    }
}