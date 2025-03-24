#include <fstream>
#include <iostream>

#include "Scene_Play.h"

#include "Scene_Menu.h"
#include "GameEngine.h"
#include "Components.h"
#include "Action.h"
#include "Physics.h"

Scene_Play::Scene_Play(GameEngine *gameEngine, const std::string &levelPath)
    : Scene(gameEngine), m_levelPath(levelPath)
{
    init(m_levelPath);
}

void Scene_Play::init(const std::string &levelPath)
{
    registerAction(sf::Keyboard::P, "PAUSE");
    registerAction(sf::Keyboard::Escape, "QUIT");
    registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");   // Toggle drawing (T)extures
    registerAction(sf::Keyboard::C, "TOGGLE_COLLISION"); // Toggle drawing (C)ollision Boxes
    registerAction(sf::Keyboard::G, "TOGGLE_GRID");      // Toggle drawing (G)rid

    // TODO: Register all other gameplay Actions
    registerAction(sf::Keyboard::A, "MOVE_LEFT");
    registerAction(sf::Keyboard::D, "MOVE_RIGHT");
    registerAction(sf::Keyboard::W, "JUMP");
    registerAction(sf::Keyboard::Space, "SHOOT");

    m_gridText.setCharacterSize(12);
    m_gridText.setFont(m_game->assets().getFont("Tech"));

    loadLevel(levelPath);
}

Vec2 Scene_Play::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
    // TODO: This function takes in a grid (x,y) position and an Entity
    // Return a Vec2 indicating where the CENTER position of the Entity should be
    // You must use the Entity's Animation size to position it correctly
    // The size of the grid width and height is stored in m_gridSize.x and m_gridSize.y
    // The bottom-left corner of the Animation should align with the bottom left of the grid cell
    if (entity->hasComponent<CAnimation>())
    {
        Vec2 halfAnimSize = entity->getComponent<CAnimation>().animation.getSize() / 2;
        Vec2 bottomLeftCell(gridX * m_gridSize.x, height() - (gridY * m_gridSize.y));

        return Vec2(bottomLeftCell.x + halfAnimSize.x, bottomLeftCell.y - halfAnimSize.y);
    }
    std::cerr << "The entity do not contain an Animation component" << std::endl;
    return Vec2(0, 0);
}

void Scene_Play::loadLevel(const std::string &filename)
{
    // reset the entity manager every time we load a level
    m_entityManager = EntityManager();

    // TODO: read in the level file and add the appropriate entities
    // use the PlayerConfig struct m_playerConfig to store player properties
    // this struct is defined at the top of Scene_Play.h

    // NOTE: all of the code below is sample code which shows you how to
    // set up and use entities with the new syntax, it should be removed

    std::ifstream fileIn(m_levelPath);
    std::string rowIdentifier;

    while (fileIn >> rowIdentifier)
    {
        if (rowIdentifier == "Tile")
        {
            std::string animName;
            float gridX, gridY;
            fileIn >> animName >> gridX >> gridY;

            Animation &animation = m_game->assets().getAnimation(animName);

            std::shared_ptr<Entity> tile = m_entityManager.addEntity("tile");
            tile->addComponent<CAnimation>(animation, true);
            tile->addComponent<CBoundingBox>(animation.getSize());
            tile->addComponent<CTransform>(gridToMidPixel(gridX, gridY, tile));
        }
        else if (rowIdentifier == "Dec")
        {
            std::string animName;
            float gridX, gridY;
            fileIn >> animName >> gridX >> gridY;

            Animation &animation = m_game->assets().getAnimation(animName);

            std::shared_ptr<Entity> dec = m_entityManager.addEntity("dec");
            dec->addComponent<CAnimation>(animation, true);
            dec->addComponent<CTransform>(gridToMidPixel(gridX, gridY, dec));
        }
        else if (rowIdentifier == "Player")
        {
            fileIn >>
                m_playerConfig.X >> m_playerConfig.Y >>
                m_playerConfig.CX >> m_playerConfig.CY >>
                m_playerConfig.SPEED >> m_playerConfig.JUMP >> m_playerConfig.MAXSPEED >>
                m_playerConfig.GRAVITY >>
                m_playerConfig.WEAPON;
        }
    }

    spawnPlayer();

    // // some sample entities
    // auto brick = m_entityManager.addEntity("tile");
    // // IMPORTANT: always add the CAnimation component first so that gridToMidPixel can compute correctly
    // brick->addComponent<CAnimation>(m_game->assets().getAnimation("Brick"), true);
    // brick->addComponent<CTransform>(Vec2(96, 480));
    // // NOTE: Your final code should position the entity with the grid x,y position read from the file:
    // // brick->addComponent<CTransform>(gridToMidPixel(gridX, gridY, brick));

    // if (brick->getComponent<CAnimation>().animation.getName() == "Brick")
    // {
    //     std::cout << "This could be a good way of identifying if a tile is a brick!\n";
    // }

    // auto block = m_entityManager.addEntity("tile");
    // block->addComponent<CAnimation>(m_game->assets().getAnimation("Ground"), true);
    // block->addComponent<CTransform>(Vec2(224, 480));
    // // add a bounding box, this will now show up if we press the 'C' key
    // block->addComponent<CBoundingBox>(m_game->assets().getAnimation("Ground").getSize());

    // auto question = m_entityManager.addEntity("tile");
    // question->addComponent<CAnimation>(m_game->assets().getAnimation("Question"), true);
    // question->addComponent<CTransform>(Vec2(352, 480));

    // NOTE: THIS IS INCREDIBLY IMPORTANT PLEASE READ THIS EXAMPLE
    // Components are now returned as references rather than pointers
    // If you do not specify a reference variable type, it will COPY the component
    // Here is an example:
    //
    // This will COPY the transform into the variable 'transform1' - it is INCORRECT
    // Any changes you make to transform1 will not be changed inside the entity
    // auto transform1 = entity->getComponent<CTransform>();
    //
    // This will REFERENCE the transform with the variable 'transform2' - it is CORRECT
    // Now any changes you make to transform2 will be changed inside the entity
    // auto& transform2 = entity->getComponent<CTransform>();
}

void Scene_Play::spawnPlayer()
{
    auto &initialAnim = m_game->assets().getAnimation("Stand");
    // here is a sample player entity which you can use to construct other entities
    m_player = m_entityManager.addEntity("player");
    m_player->addComponent<CAnimation>(initialAnim, true);
    m_player->addComponent<CTransform>(gridToMidPixel(m_playerConfig.X, m_playerConfig.Y, m_player));
    m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CX, m_playerConfig.CY));

    // TODO: be sure to add the remaining components to the player
    m_player->addComponent<CGravity>(m_playerConfig.GRAVITY);
    m_player->addComponent<CState>("idle");
}

void Scene_Play::spawnBullet(std::shared_ptr<Entity> entity)
{
    // TODO: this should spawn a bullet at the given entity, going in the direction the entity is facing
}

void Scene_Play::update()
{
    m_entityManager.update();
    // TODO: implement pause functionality

    sMovement();
    sLifespan();
    sCollision();
    sAnimation();
    sRender();
}

void Scene_Play::sMovement()
{
    // NOTE: Setting an entity's scale.x to -1/1 will make it face to the left/right
    CTransform &transform = m_player->getComponent<CTransform>();
    CInput &input = m_player->getComponent<CInput>();

    // TODO: Implement player movement / jumping based on its CInput component
    if (input.left)
    {
        transform.velocity.x -= m_playerConfig.SPEED;
        transform.scale.x = -1;
    }
    else if (input.right)
    {
        transform.velocity.x += m_playerConfig.SPEED;
        transform.scale.x = 1;
    }
    else if (!input.left && !input.right)
    {
        transform.velocity.x = 0;
    }

    if (input.up)
    {
        if (input.canJump)
        {
            m_player->addComponent<CState>("OnAir");
            input.canJump = false;
            transform.velocity.y -= m_playerConfig.JUMP;
        }
    }

    // TODO: Implement gravity's effect on the player
    float gravity = m_player->getComponent<CGravity>().gravity;
    transform.velocity.y += gravity;

    // TODO: Implement the maximum player speed in both X and Y directions
    if (transform.velocity.x > m_playerConfig.MAXSPEED)
    {
        transform.velocity.x = m_playerConfig.MAXSPEED;
    }
    else if (transform.velocity.x < -m_playerConfig.MAXSPEED)
    {
        transform.velocity.x = -m_playerConfig.MAXSPEED;
    }

    if (transform.velocity.y > m_playerConfig.MAXSPEED)
    {
        transform.velocity.y = m_playerConfig.MAXSPEED;
    }
    else if (transform.velocity.y < -m_playerConfig.MAXSPEED)
    {
        transform.velocity.y = -m_playerConfig.MAXSPEED;
    }

    transform.prevPos = transform.pos;
    transform.pos += transform.velocity;
}

void Scene_Play::sLifespan()
{
    // TODO: Check lifespan of entities that have them, and destroy them if they go over
}

void Scene_Play::sCollision()
{
    // REMEMBER: SFML's (0,0) position is on the TOP-LEFT corner
    // This means jumping will have a negative y-component
    // and gravity will have a positive y-component
    // Also, something BELOW something else will have a y value GREATER than it
    // Also, something ABOVE something else will have a y value LESS than it

    // TODO: Implement Physics::GetOverlap() function, use it inside this function

    // TODO: Implement bullet / tile collisions
    // Destroy the tile if it has a Brick animation
    for (std::shared_ptr<Entity> bullet : m_entityManager.getEntities("bullet"))
    {
        for (std::shared_ptr<Entity> tile : m_entityManager.getEntities("tile"))
        {
            Vec2 overlap = Physics::GetOverlap(bullet, tile);
        }
    }

    // TODO: Implement player / tile collisions and resolutions
    // Update the CState component of the player to store whether
    // it is currently on the ground or in the air. This will be
    // used by the Animation system
    bool isOnAir = true;
    for (std::shared_ptr<Entity> tile : m_entityManager.getEntities("tile"))
    {
        Vec2 overlap = Physics::GetOverlap(m_player, tile);
        if (overlap.y >= 0)
        {
        }
    }

    // TODO: Check to see if the player has fallen down a hole (y > height())
    // TODO: Don't let the player walk off the left side of the map
    auto &position = m_player->getComponent<CTransform>().pos;
    auto halfSize = m_player->getComponent<CAnimation>().animation.getSize() / 2;
    if (position.x - halfSize.x < 0)
    {
        position.x = halfSize.x;
    }

    // HACK to stop falling out the screen
    if (position.y + halfSize.y > height())
    {
        position.y = height() - halfSize.y;
        m_player->addComponent<CState>("onFloor");
        m_player->getComponent<CInput>().canJump = true;
    }
}

void Scene_Play::sDoAction(const Action &action)
{
    if (action.type() == "START")
    {
        if (action.name() == "TOGGLE_TEXTURE")
        {
            m_drawTextures = !m_drawTextures;
        }
        else if (action.name() == "TOGGLE_COLLISION")
        {
            m_drawCollision = !m_drawCollision;
        }
        else if (action.name() == "TOGGLE_GRID")
        {
            m_drawGrid = !m_drawGrid;
        }
        else if (action.name() == "PAUSE")
        {
            setPaused(!m_paused);
        }
        else if (action.name() == "QUIT")
        {
            onEnd();
        }
        else if (action.name() == "MOVE_LEFT")
        {
            m_player->getComponent<CInput>().left = true;
        }
        else if (action.name() == "MOVE_RIGHT")
        {
            m_player->getComponent<CInput>().right = true;
        }
        else if (action.name() == "JUMP")
        {
            m_player->getComponent<CInput>().up = true;
        }
        else if (action.name() == "SHOOT")
        {
            m_player->getComponent<CInput>().shoot = true;
        }
    }
    else if (action.type() == "END")
    {
        if (action.name() == "MOVE_LEFT")
        {
            m_player->getComponent<CInput>().left = false;
        }
        else if (action.name() == "MOVE_RIGHT")
        {
            m_player->getComponent<CInput>().right = false;
        }
        else if (action.name() == "JUMP")
        {
            m_player->getComponent<CInput>().up = false;
        }
        else if (action.name() == "SHOOT")
        {
            m_player->getComponent<CInput>().shoot = false;
        }
    }
}

void Scene_Play::sAnimation()
{
    // TODO: Complete the Animation class code first

    // TODO: set the animation of the player based on its CState component
    // TODO: for each entity with an animation, call entity->getComponent<CAnimation>().animation.update()
    //       if the animation is not repeated, and it has ended, destroy the entity
}

void Scene_Play::onEnd()
{
    // TODO: When the scene ends, change back to the MENU scene
    //       use m_game->changeScene(correct params);
    m_game->changeScene("menu", std::make_shared<Scene_Menu>(m_game));
}

void Scene_Play::sRender()
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

    // Center the viewport on the player (if far enough right)
    auto &pPos = m_player->getComponent<CTransform>().pos; // Use reference for efficiency

    float windowCenterX = std::max(m_game->window().getSize().x / 2.0f, pPos.x);

    sf::View view = m_game->window().getView();

    // Corrected view center setting:
    view.setCenter(windowCenterX, view.getCenter().y); // Keep the current y-center

    m_game->window().setView(view);

    // Draw all Entity textures/animations
    if (m_drawTextures)
    {
        for (auto &e : m_entityManager.getEntities()) // Use a reference for efficiency
        {
            auto &transform = e->getComponent<CTransform>(); // Use a reference for efficiency

            if (e->hasComponent<CAnimation>())
            {
                auto &animation = e->getComponent<CAnimation>().animation; // Use a reference!
                auto &sprite = animation.getSprite();                      // Get a reference to the sprite

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
        for (auto &e : m_entityManager.getEntities())
        {
            if (e->hasComponent<CBoundingBox>())
            {
                auto &box = e->getComponent<CBoundingBox>();
                auto &transform = e->getComponent<CTransform>();

                sf::RectangleShape rect;
                rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1)); // Subtract 1 for visibility
                rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
                rect.setPosition(transform.pos.x, transform.pos.y);
                rect.setFillColor(sf::Color(0, 0, 0, 0));       // Transparent fill
                rect.setOutlineColor(sf::Color(255, 255, 255)); // White outline
                rect.setOutlineThickness(1);

                m_game->window().draw(rect);
            }
        }
    }

    // Draw the grid for debugging
    if (m_drawGrid)
    {
        float leftX = m_game->window().getView().getCenter().x - width() / 2.0f; // Use 2.0f for float division
        float rightX = leftX + width() + m_gridSize.x;

        float nextGridX = leftX - ((int)leftX % (int)m_gridSize.x);

        for (float x = nextGridX; x < rightX; x += m_gridSize.x)
        {
            drawLine(Vec2(x, 0), Vec2(x, height()));
        }

        for (float y = 0; y < height(); y += m_gridSize.y)
        {
            drawLine(Vec2(leftX, height() - y), Vec2(rightX, height() - y));

            for (float x = nextGridX; x < rightX; x += m_gridSize.x)
            {
                std::string xCell = std::to_string((int)x / (int)m_gridSize.x);
                std::string yCell = std::to_string((int)y / (int)m_gridSize.y);

                m_gridText.setString("(" + xCell + "," + yCell + ")");
                m_gridText.setPosition(x + 3, height() - y - m_gridSize.y + 2); // Corrected positioning

                m_game->window().draw(m_gridText);
            }
        }
    }
}