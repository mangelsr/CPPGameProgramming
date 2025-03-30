#include <fstream>
#include <iostream>

#include "Scene_Play.h"

#include "Scene_Menu.h"
#include "GameEngine.h"
#include "Components.h"
#include "Action.h"
#include "Physics.h"

bool IsInside(Vec2 pos, std::shared_ptr<Entity> entity)
{
    Vec2 ePos = entity->getComponent<CTransform>().pos;
    Vec2 halfSize = entity->getComponent<CAnimation>().animation.getSize() / 2;

    float dx = std::abs(pos.x - ePos.x);
    float dy = std::abs(pos.y - ePos.y);

    return (dx <= halfSize.x) && (dy <= halfSize.y);
}

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

    std::ifstream fileIn(filename);
    std::string rowIdentifier;

    while (fileIn >> rowIdentifier)
    {
        if (rowIdentifier == "Tile")
        {
            std::string animName;
            float gridX, gridY;
            fileIn >> animName >> gridX >> gridY;

            Animation &animation = m_game->assets().getAnimation(animName);

            std::shared_ptr<Entity> tile = m_entityManager.addEntity("Tile");
            tile->addComponent<CAnimation>(animation, true);
            tile->addComponent<CBoundingBox>(animation.getSize());
            tile->addComponent<CTransform>(gridToMidPixel(gridX, gridY, tile));
            tile->addComponent<CDraggable>();
        }
        else if (rowIdentifier == "Dec")
        {
            std::string animName;
            float gridX, gridY;
            fileIn >> animName >> gridX >> gridY;

            Animation &animation = m_game->assets().getAnimation(animName);

            std::shared_ptr<Entity> dec = m_entityManager.addEntity("Dec");
            dec->addComponent<CAnimation>(animation, true);
            dec->addComponent<CTransform>(gridToMidPixel(gridX, gridY, dec));
            dec->addComponent<CDraggable>();
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
}

void Scene_Play::spawnPlayer()
{
    Animation &initialAnim = m_game->assets().getAnimation("Stand");

    m_player = m_entityManager.addEntity("Player");
    m_player->addComponent<CAnimation>(initialAnim, true);
    m_player->addComponent<CTransform>(gridToMidPixel(m_playerConfig.X, m_playerConfig.Y, m_player));
    m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CX, m_playerConfig.CY));

    m_player->addComponent<CGravity>(m_playerConfig.GRAVITY);
    m_player->addComponent<CState>("Stand", true);
}

void Scene_Play::spawnBullet(std::shared_ptr<Entity> entity)
{
    float bulletSpeed = 10; // pixels per frame
    int lifeSpanTime = 10;  // in seconds
    int lifetimeFrames = m_game->framerateLimit() * lifeSpanTime;

    Animation &animation = m_game->assets().getAnimation(m_playerConfig.WEAPON);
    CTransform &eTransform = entity->getComponent<CTransform>();

    Vec2 position(eTransform.pos);
    Vec2 velocity(0, 0);
    if (eTransform.scale.x == 1)
    {
        velocity.x = bulletSpeed;
    }
    else if (eTransform.scale.x == -1)
    {
        velocity.x = -bulletSpeed;
    }

    std::shared_ptr<Entity> bullet = m_entityManager.addEntity("Bullet");
    bullet->addComponent<CAnimation>(animation, true);
    bullet->addComponent<CBoundingBox>(animation.getSize());
    bullet->addComponent<CTransform>(position, velocity, Vec2(1, 1), 0);
    bullet->addComponent<CLifeSpan>(lifetimeFrames, m_currentFrame);
}

void Scene_Play::update()
{
    m_entityManager.update();

    if (!m_paused)
    {
        sMovement();
        sState();
        sLifespan();
        sCollision();
        sAnimation();
        sDragAndDrop();
    }
    sRender();
}

void Scene_Play::sMovement()
{
    CTransform &transform = m_player->getComponent<CTransform>();
    CInput &input = m_player->getComponent<CInput>();
    CState &state = m_player->getComponent<CState>();

    transform.velocity.x = 0;
    if (input.left)
    {
        transform.velocity.x -= m_playerConfig.SPEED;
        transform.scale.x = -1;
    }
    if (input.right)
    {
        transform.velocity.x += m_playerConfig.SPEED;
        transform.scale.x = 1;
    }

    if (input.up && input.canJump)
    {
        transform.velocity.y = -m_playerConfig.JUMP;
        input.canJump = false;
        state.onGround = false;
        state.animation = "Jump";
    }

    transform.velocity.y += m_player->getComponent<CGravity>().gravity;

    if (input.shoot && input.canShoot)
    {
        spawnBullet(m_player);
        input.canShoot = false;
    }
    else if (!input.shoot)
    {
        input.canShoot = true;
    }

    transform.velocity.x = std::clamp(transform.velocity.x, -m_playerConfig.MAXSPEED, m_playerConfig.MAXSPEED);
    transform.velocity.y = std::clamp(transform.velocity.y, -m_playerConfig.MAXSPEED, m_playerConfig.MAXSPEED);

    transform.prevPos = transform.pos;
    transform.pos += transform.velocity;

    for (const std::shared_ptr<Entity> &bullet : m_entityManager.getEntities("Bullet"))
    {
        CTransform &bTransform = bullet->getComponent<CTransform>();
        bTransform.prevPos = bTransform.pos;
        bTransform.pos += bTransform.velocity;
    }
}

void Scene_Play::sState()
{
    CInput &input = m_player->getComponent<CInput>();
    CState &state = m_player->getComponent<CState>();

    std::string newAnimation = state.animation;

    if (!state.onGround)
    {
        newAnimation = input.shoot ? "AirShoot" : "Jump";
    }
    else if (input.left || input.right)
    {
        newAnimation = input.shoot ? "RunShoot" : "Run";
    }
    else if (input.shoot)
    {
        newAnimation = "StandShoot";
    }
    else
    {
        newAnimation = "Stand";
    }

    if (newAnimation != state.animation)
    {
        state.animation = newAnimation;
    }
}

void Scene_Play::sLifespan()
{
    for (const std::shared_ptr<Entity> &e : m_entityManager.getEntities())
    {
        if (e->hasComponent<CLifeSpan>())
        {
            CLifeSpan &lifeSpan = e->getComponent<CLifeSpan>();
            if (int(m_currentFrame) > lifeSpan.frameCreated + lifeSpan.lifespan)
            {
                e->destroy();
            }
        }
    }
}

void Scene_Play::sCollision()
{
    Animation &explosionAnim = m_game->assets().getAnimation("Explosion");

    for (const std::shared_ptr<Entity> &bullet : m_entityManager.getEntities("Bullet"))
    {
        if (bullet->isActive())
        {
            for (const std::shared_ptr<Entity> &tile : m_entityManager.getEntities("Tile"))
            {
                Vec2 overlap = Physics::GetOverlap(bullet, tile);
                if (overlap.x >= 0 && overlap.y >= 0)
                {
                    std::string animName = tile->getComponent<CAnimation>().animation.getName();
                    if (animName == "Brick")
                        tile->addComponent<CAnimation>(explosionAnim, false);
                    bullet->destroy();
                }
            }
        }
    }

    CState &pState = m_player->getComponent<CState>();
    CTransform &pTransform = m_player->getComponent<CTransform>();
    CInput &pInput = m_player->getComponent<CInput>();
    CBoundingBox &pBBox = m_player->getComponent<CBoundingBox>();

    pState.onGround = false;

    for (const std::shared_ptr<Entity> &tile : m_entityManager.getEntities("Tile"))
    {
        Vec2 overlap = Physics::GetOverlap(m_player, tile);
        Vec2 prevOverlap = Physics::GetPreviousOverlap(m_player, tile);

        if (overlap.x > 0 && overlap.y > 0)
        {
            Vec2 tilePos = tile->getComponent<CTransform>().pos;
            Vec2 tileHalfSize = tile->getComponent<CBoundingBox>().halfSize;

            float dx = pTransform.pos.x - tilePos.x;
            float dy = pTransform.pos.y - tilePos.y;

            float xOverlap = pBBox.halfSize.x + tileHalfSize.x - abs(dx);
            float yOverlap = pBBox.halfSize.y + tileHalfSize.y - abs(dy);

            if (xOverlap < yOverlap)
            {
                if (dx > 0)
                {
                    pTransform.pos.x += xOverlap;
                }
                else
                {
                    pTransform.pos.x -= xOverlap;
                }
                pTransform.velocity.x = 0;
            }
            else
            {
                if (dy > 0)
                {
                    pTransform.pos.y += yOverlap;

                    std::string animName = tile->getComponent<CAnimation>().animation.getName();
                    if (animName == "Question")
                    {
                        Animation &coinAnimation = m_game->assets().getAnimation("CoinSpin");
                        std::shared_ptr<Entity> coinEntity = m_entityManager.addEntity("Coin");
                        coinEntity->addComponent<CAnimation>(coinAnimation, true);
                        coinEntity->addComponent<CLifeSpan>(30, m_currentFrame);
                        coinEntity->addComponent<CTransform>(Vec2(tilePos.x, tilePos.y - 64));

                        Animation &questionHit = m_game->assets().getAnimation("QuestionHit");
                        tile->addComponent<CAnimation>(questionHit, true);
                    }

                    if (pTransform.velocity.y < 0)
                        pTransform.velocity.y = 0;
                }
                else
                {
                    pTransform.pos.y -= yOverlap;
                    pTransform.velocity.y = 0;
                    pState.onGround = true;
                    pInput.canJump = true;

                    if (prevOverlap.y <= 0 && pTransform.velocity.y > 0)
                    {
                        pState.animation = (abs(pTransform.velocity.x) > 0) ? "Run" : "Stand";
                    }
                }
            }
        }
    }

    if (!pState.onGround)
    {
        if (abs(pTransform.velocity.y) > 0)
        {
            pState.animation = (pInput.shoot) ? "AirShoot" : "Jump";
        }
    }

    if (pTransform.pos.y > height())
    {
        init(m_levelPath);
    }

    if (pTransform.pos.x < 0)
    {
        pTransform.pos.x = 0;
        pTransform.velocity.x = 0;
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

        else if (action.name() == "LEFT_CLICK")
        {
            Vec2 worldPos = windowToWorld(m_mousePos);
            for (auto &e : m_entityManager.getEntities())
            {
                if (e->hasComponent<CDraggable>() && IsInside(worldPos, e))
                {
                    std::cout << "Clicked Entity: " << e->getComponent<CAnimation>().animation.getName() << std::endl;
                    e->getComponent<CDraggable>().dragging = !e->getComponent<CDraggable>().dragging;
                }
            }
        }
        else if (action.name() == "MOUSE_MOVE")
        {
            m_mousePos = action.pos();
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

void Scene_Play::sDragAndDrop()
{
    for (auto &e : m_entityManager.getEntities())
    {
        if (e->hasComponent<CDraggable>() && e->getComponent<CDraggable>().dragging)
        {
            Vec2 worldPos = windowToWorld(m_mousePos);
            e->getComponent<CTransform>().pos = worldPos;
        }
    }
}

void Scene_Play::sAnimation()
{
    std::string playerState = m_player->getComponent<CState>().animation;
    CAnimation &currentAnimation = m_player->getComponent<CAnimation>();

    if (playerState != currentAnimation.animation.getName())
    {
        Animation &anim = m_game->assets().getAnimation(playerState);
        m_player->addComponent<CAnimation>(anim, true);
    }

    for (const std::shared_ptr<Entity> &e : m_entityManager.getEntities())
    {
        if (e->hasComponent<CAnimation>())
        {
            CAnimation &anim = e->getComponent<CAnimation>();
            anim.animation.update();
            if (!anim.repeat && anim.animation.hasEnded())
                e->destroy();
        }
    }
}

void Scene_Play::onEnd()
{
    m_hasEnded = true;
    m_game->changeScene("menu", std::make_shared<Scene_Menu>(m_game));
}

// This method only works on games that do not rotate or zoom-in/out the view
// just scroll on x and/or y
Vec2 Scene_Play::windowToWorld(const Vec2 &window) const
{
    sf::RenderWindow &win = m_game->window();
    sf::Vector2u winSize = win.getSize();
    sf::Vector2f viewCenter = win.getView().getCenter();

    float wx = viewCenter.x - (winSize.x / 2);
    float wy = viewCenter.y - (winSize.y / 2);

    return Vec2(window.x + wx, window.y + wy);
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
    Vec2 &pPos = m_player->getComponent<CTransform>().pos; // Use reference for efficiency

    float windowCenterX = std::max(m_game->window().getSize().x / 2.0f, pPos.x);

    sf::View view = m_game->window().getView();

    // Corrected view center setting:
    view.setCenter(windowCenterX, view.getCenter().y); // Keep the current y-center

    m_game->window().setView(view);

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

    Vec2 worldPos = windowToWorld(m_mousePos);
    m_mouseShape.setFillColor(sf::Color(255, 0, 0));
    m_mouseShape.setRadius(4);
    m_mouseShape.setOrigin(2, 2);
    m_mouseShape.setPosition(worldPos.x, worldPos.y);
    m_game->window().draw(m_mouseShape);

    if (!m_paused)
        m_currentFrame++;
}