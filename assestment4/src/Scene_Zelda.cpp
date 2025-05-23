#include "fstream"

#include "Scene_Zelda.h"
#include "Scene_Menu.h"
#include "Physics.h"

Scene_Zelda::Scene_Zelda(GameEngine *gameEngine, const std::string &levelPath)
    : Scene(gameEngine), m_levelPath(levelPath)
{
    init(m_levelPath);
}

void Scene_Zelda::init(const std::string &levelPath)
{
    loadLevel(levelPath);

    registerAction(sf::Keyboard::Escape, "QUIT");
    registerAction(sf::Keyboard::P, "PAUSE");
    registerAction(sf::Keyboard::Y, "TOGGLE_FOLLOW");    // Toggle Follow/Room Camera
    registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");   // Toggle drawing (T)extures
    registerAction(sf::Keyboard::C, "TOGGLE_COLLISION"); // Toggle drawing (C)ollision Boxes

    registerAction(sf::Keyboard::W, "MOVE_UP");
    registerAction(sf::Keyboard::A, "MOVE_LEFT");
    registerAction(sf::Keyboard::S, "MOVE_DOWN");
    registerAction(sf::Keyboard::D, "MOVE_RIGHT");
    registerAction(sf::Keyboard::Space, "ATTACK");
}

void Scene_Zelda::loadLevel(const std::string &filename)
{
    m_entityManager = EntityManager();

    std::ifstream fileIn(filename);
    std::string rowIdentifier;

    while (fileIn >> rowIdentifier)
    {
        if (rowIdentifier == "Tile")
        {
            std::string animName;
            int rx, ry, tx, ty;
            bool blockMovement, blockVision;
            fileIn >> animName >> rx >> ry >> tx >> ty >> blockMovement >> blockVision;

            Animation &animation = m_game->assets().getAnimation(animName);

            std::string tag = "Tile";
            if (animName == "TileHeart")
                tag = "Heart";
            else if (animName == "TileBlack")
                tag = "Portal";

            std::shared_ptr<Entity> tile = m_entityManager.addEntity(tag);
            tile->addComponent<CAnimation>(animation, true);
            tile->addComponent<CBoundingBox>(animation.getSize(), blockMovement, blockVision);
            tile->addComponent<CTransform>(getPosition(rx, ry, tx, ty));
        }
        else if (rowIdentifier == "NPC")
        {
            std::string animName;
            int rx, ry, tx, ty;
            bool blockMovement, blockVision;
            int health, damage;
            std::string behavior;

            fileIn >> animName >> rx >> ry >> tx >> ty >>
                blockMovement >> blockVision >> health >> damage >> behavior;

            Animation &animation = m_game->assets().getAnimation(animName);

            Vec2 initialPosition = getPosition(rx, ry, tx, ty);

            std::shared_ptr<Entity> npc = m_entityManager.addEntity("NPC");
            npc->addComponent<CAnimation>(animation, true);
            npc->addComponent<CBoundingBox>(animation.getSize(), blockMovement, blockVision);
            npc->addComponent<CTransform>(initialPosition);
            npc->addComponent<CHealth>(health, health);
            npc->addComponent<CDamage>(damage);

            if (behavior == "Follow")
            {
                int speed;
                fileIn >> speed;
                npc->addComponent<CFollowPlayer>(initialPosition, speed);
            }
            else if (behavior == "Patrol")
            {
                int speed, waypointsNumber;
                std::vector<Vec2> waypoints;

                fileIn >> speed >> waypointsNumber;
                waypoints.reserve(waypointsNumber);
                for (int i = 0; i < waypointsNumber; i++)
                {
                    int x, y;
                    fileIn >> x >> y;
                    waypoints.push_back(getPosition(rx, ry, x, y));
                }
                npc->addComponent<CPatrol>(waypoints, speed);
            }
        }
        else if (rowIdentifier == "Player")
        {
            fileIn >>
                m_playerConfig.X >> m_playerConfig.Y >>
                m_playerConfig.CX >> m_playerConfig.CY >>
                m_playerConfig.SPEED >> m_playerConfig.HEALTH;
        }
    }

    spawnPlayer();
}

Vec2 Scene_Zelda::getPosition(int rx, int ry, int tx, int ty) const
{
    int tileSize = 64;
    sf::Vector2u size = m_game->window().getSize();
    return Vec2(
        (rx * int(size.x)) + (tx * tileSize / 2),
        (ry * int(size.y)) + (ty * tileSize / 2));
}

void Scene_Zelda::spawnPlayer()
{
    m_player = m_entityManager.addEntity("Player");
    m_player->addComponent<CTransform>(Vec2(m_playerConfig.X, m_playerConfig.Y));
    m_player->addComponent<CAnimation>(m_game->assets().getAnimation("LinkStandDown"), true);
    m_player->addComponent<CState>("LinkStandDown");
    m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CX, m_playerConfig.CY), true, false);
    m_player->addComponent<CHealth>(m_playerConfig.HEALTH, m_playerConfig.HEALTH);
}

void Scene_Zelda::spawnSword(std::shared_ptr<Entity> entity)
{
    CInput &input = entity->getComponent<CInput>();
    if (!input.canAttack)
        return;

    Animation &swordAnimation = m_game->assets().getAnimation("Sword");

    std::shared_ptr<Entity> sword = m_entityManager.addEntity("Sword");
    sword->addComponent<CLifeSpan>(30, m_currentFrame);
    sword->addComponent<CDamage>(1);
    sword->addComponent<CAnimation>(swordAnimation, true);
    sword->addComponent<CTransform>();

    locateSword(m_player, sword);

    sf::Sound &slashSfx = m_game->assets().getSound("SSwordSlash");
    slashSfx.play();

    input.canAttack = false;
}

void Scene_Zelda::update()
{
    m_entityManager.update();

    if (!m_paused)
    {
        sAI();
        sMovement();
        sStatus();
        sCollision();
        sAnimation();
        sCamera();

        m_currentFrame++;
    }
}

void Scene_Zelda::onEnd()
{
    m_hasEnded = true;
}

void Scene_Zelda::locateSword(std::shared_ptr<Entity> wielder, std::shared_ptr<Entity> sword)
{
    CTransform &wielderTransform = wielder->getComponent<CTransform>();
    std::string wielderState = wielder->getComponent<CState>().animation;

    CTransform &swordTransform = sword->getComponent<CTransform>();

    Animation &swordAnimation = m_game->assets().getAnimation("Sword");
    Vec2 spriteSize = swordAnimation.getSize();

    Vec2 swordPos = wielderTransform.pos;
    Vec2 swordScale(1, 1);
    float swordAngle = 0;

    if (wielderState.find("Up") != std::string::npos)
    {
        swordPos.y -= spriteSize.y;
    }
    else if (wielderState.find("Down") != std::string::npos)
    {
        swordScale.y = -1;
        swordPos.y += spriteSize.y;
    }
    else if (wielderState.find("Right") != std::string::npos)
    {
        swordScale.x = wielderTransform.scale.x;
        swordAngle = (wielderTransform.scale.x == 1) ? 90 : -90;
        swordPos.x += (wielderTransform.scale.x == 1) ? spriteSize.y : -spriteSize.y;
    }

    swordTransform.scale = swordScale;
    swordTransform.angle = swordAngle;
    swordTransform.pos = swordPos;

    Vec2 swordBBox = swordAngle != 0 ? Vec2(spriteSize.y, spriteSize.x) : Vec2(spriteSize.x, spriteSize.y);
    sword->addComponent<CBoundingBox>(swordBBox);

    swordTransform.pos += wielderTransform.velocity;
}

void Scene_Zelda::sMovement()
{
    CInput &input = m_player->getComponent<CInput>();
    CTransform &transform = m_player->getComponent<CTransform>();
    CState &state = m_player->getComponent<CState>();

    if (input.up && input.down)
    {
        transform.velocity = {0, 0};
        auto idx = state.animation.find("Move");
        if (idx != std::string::npos)
        {
            state.animation.replace(idx, 4, "Stand");
        }
    }

    else if (input.up)
    {
        if (!input.left && !input.right)
        {
            transform.velocity = {0, 0};
            transform.velocity.y = -m_playerConfig.SPEED;
            state.animation = "LinkMoveUp";
        }
    }

    else if (input.down)
    {
        if (!input.left && !input.right)
        {
            transform.velocity = {0, 0};
            transform.velocity.y = m_playerConfig.SPEED;
            state.animation = "LinkMoveDown";
        }
    }

    if (input.left && input.right)
    {
        transform.velocity = {0, 0};
        auto idx = state.animation.find("Move");
        if (idx != std::string::npos)
        {
            state.animation.replace(idx, 4, "Stand");
        }
    }

    else if (input.left)
    {
        if (!input.up && !input.down)
        {
            transform.velocity = {0, 0};
            transform.scale.x = -1;
            transform.velocity.x = -m_playerConfig.SPEED;
            state.animation = "LinkMoveRight";
        }
    }

    else if (input.right)
    {
        if (!input.up && !input.down)
        {
            transform.velocity = {0, 0};
            transform.scale.x = 1;
            transform.velocity.x = m_playerConfig.SPEED;
            state.animation = "LinkMoveRight";
        }
    }

    if (!input.up && !input.down && !input.left && !input.right)
    {
        transform.velocity = {0, 0};
        auto idx = state.animation.find("Move");
        if (idx != std::string::npos)
        {
            state.animation.replace(idx, 4, "Stand");
        }
    }

    if (input.attack)
    {
        size_t idx = state.animation.find("Move");
        if (idx != std::string::npos)
        {
            state.animation.replace(idx, 4, "Atk");
        }
        idx = state.animation.find("Stand");
        if (idx != std::string::npos)
        {
            state.animation.replace(idx, 5, "Atk");
        }
    }

    transform.pos += transform.velocity;
}

void Scene_Zelda::sDoAction(const Action &action)
{
    CInput &input = m_player->getComponent<CInput>();
    if (action.type() == "START")
    {
        if (action.name() == "PAUSE")
        {
            setPaused(!m_paused);
        }
        else if (action.name() == "QUIT")
        {
            m_game->changeScene("menu", std::make_shared<Scene_Menu>(m_game));
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
        else if (action.name() == "MOVE_UP")
        {
            input.up = true;
        }
        else if (action.name() == "MOVE_LEFT")
        {
            input.left = true;
        }
        else if (action.name() == "MOVE_DOWN")
        {
            input.down = true;
        }
        else if (action.name() == "MOVE_RIGHT")
        {
            input.right = true;
        }
        else if (action.name() == "ATTACK")
        {
            input.attack = true;
            spawnSword(m_player);
        }
    }
    else if (action.type() == "END")
    {
        if (action.name() == "MOVE_UP")
        {
            input.up = false;
        }
        else if (action.name() == "MOVE_LEFT")
        {
            input.left = false;
        }
        else if (action.name() == "MOVE_DOWN")
        {
            input.down = false;
        }
        else if (action.name() == "MOVE_RIGHT")
        {
            input.right = false;
        }
        else if (action.name() == "ATTACK")
        {
            input.attack = false;
        }
    }
}

void Scene_Zelda::sAI()
{
    Vec2 &playerPos = m_player->getComponent<CTransform>().pos;

    for (const std::shared_ptr<Entity> &npc : m_entityManager.getEntities("NPC"))
    {
        // Implement Follow behavior
        if (npc->hasComponent<CFollowPlayer>())
        {
            CFollowPlayer &follow = npc->getComponent<CFollowPlayer>();
            CTransform &transform = npc->getComponent<CTransform>();

            bool hasLineSight = true;
            for (const std::shared_ptr<Entity> &otherEntity : m_entityManager.getEntities())
            {
                if (otherEntity != m_player &&
                    otherEntity != npc &&
                    otherEntity->hasComponent<CBoundingBox>() &&
                    otherEntity->getComponent<CBoundingBox>().blockVision)
                {
                    if (Physics::EntityIntersect(playerPos, transform.pos, otherEntity))
                    {
                        hasLineSight = false;
                        break;
                    }
                }
            }

            Vec2 movement;
            if (hasLineSight)
                movement = (playerPos - transform.pos).normalize() * follow.speed;

            else
                movement = (follow.home - transform.pos).normalize() * follow.speed;

            transform.prevPos = transform.pos;
            transform.pos += movement;
        }

        // Implement Patrol behavior
        if (npc->hasComponent<CPatrol>())
        {
            CPatrol &patrol = npc->getComponent<CPatrol>();
            CTransform &transform = npc->getComponent<CTransform>();

            Vec2 origin = transform.pos;
            Vec2 destination = patrol.positions[patrol.currentPosition];

            Vec2 diff = destination - origin;

            if (int(diff.x) == 0 && int(diff.y) == 0)
            {
                patrol.currentPosition = (patrol.currentPosition + 1) % patrol.positions.size();
            }
            else
            {
                Vec2 movement = diff.normalize() * patrol.speed;
                transform.prevPos = transform.pos;
                transform.pos += movement;
            }
        }
    }
}

void Scene_Zelda::sStatus()
{
    for (const std::shared_ptr<Entity> &e : m_entityManager.getEntities())
    {
        if (e->hasComponent<CLifeSpan>())
        {
            CLifeSpan &lifeSpan = e->getComponent<CLifeSpan>();
            if (int(m_currentFrame) > lifeSpan.frameCreated + lifeSpan.lifespan)
            {
                if (e->tag() == "Sword")
                {
                    m_player->getComponent<CInput>().canAttack = true;
                    CState &state = m_player->getComponent<CState>();
                    size_t idx = state.animation.find("Atk");
                    if (idx != std::string::npos)
                    {
                        state.animation = state.animation.replace(idx, 3, "Stand");
                    }
                }
                e->destroy();
            }
        }
    }

    for (const std::shared_ptr<Entity> &e : m_entityManager.getEntities())
    {
        if (e->hasComponent<CInvincibility>())
        {
            CInvincibility &invincibility = e->getComponent<CInvincibility>();
            if (invincibility.iframes <= 0)
            {
                e->removeComponent<CInvincibility>();
                Animation &animation = e->getComponent<CAnimation>().animation;
                sf::Sprite &sprite = animation.getSprite();
                sprite.setColor(sf::Color(255, 255, 255, 255));
            }
            else
                invincibility.iframes--;
        }
    }
}

void Scene_Zelda::sCollision()
{
    // Implement entity - tile collisions
    entityTileCollisions();

    // Implement player - enemy collisions with appropriate damage calculations
    playerEnemyCollisions();

    // Implement Sword - NPC collisions
    swordNPCCollisions();

    // Implement black tile collisions / 'teleporting'
    blackTileCollisions();

    // Implement entity - heart collisions and life gain logic
    entityHeartCollisions();
}

void Scene_Zelda::entityTileCollisions()
{
    for (const std::shared_ptr<Entity> &entity : m_entityManager.getEntities())
    {
        if (entity->tag() != "Tile")
        {
            CTransform &eTransform = entity->getComponent<CTransform>();
            CBoundingBox &eBBox = entity->getComponent<CBoundingBox>();

            for (const std::shared_ptr<Entity> &tile : m_entityManager.getEntities("Tile"))
            {
                if (tile->hasComponent<CBoundingBox>() && tile->getComponent<CBoundingBox>().blockMove)
                {
                    Vec2 overlap = Physics::GetOverlap(entity, tile);

                    if (overlap.x > 0 && overlap.y > 0)
                    {
                        Vec2 tilePos = tile->getComponent<CTransform>().pos;
                        Vec2 tileHalfSize = tile->getComponent<CBoundingBox>().halfSize;

                        float dx = eTransform.pos.x - tilePos.x;
                        float dy = eTransform.pos.y - tilePos.y;

                        float xOverlap = eBBox.halfSize.x + tileHalfSize.x - abs(dx);
                        float yOverlap = eBBox.halfSize.y + tileHalfSize.y - abs(dy);

                        if (xOverlap < yOverlap)
                        {
                            if (dx > 0)
                            {
                                eTransform.pos.x += xOverlap;
                            }
                            else
                            {
                                eTransform.pos.x -= xOverlap;
                            }
                        }
                        else
                        {
                            if (dy > 0)
                            {
                                eTransform.pos.y += yOverlap;
                            }
                            else
                            {
                                eTransform.pos.y -= yOverlap;
                            }
                        }
                    }
                }
            }
        }
    }
}

void Scene_Zelda::playerEnemyCollisions()
{
    if (!m_player->hasComponent<CInvincibility>())
    {
        for (const std::shared_ptr<Entity> &enemy : m_entityManager.getEntities("NPC"))
        {
            Vec2 overlap = Physics::GetOverlap(enemy, m_player);
            if (overlap.x > 0 && overlap.y > 0)
            {
                CHealth &health = m_player->getComponent<CHealth>();
                health.current -= enemy->getComponent<CDamage>().damage;

                Animation &animation = m_player->getComponent<CAnimation>().animation;
                sf::Sprite &sprite = animation.getSprite();
                sprite.setColor(sf::Color(255, 255, 255, 128));

                if (health.current <= 0)
                {
                    sf::Sound &sfx = m_game->assets().getSound("SLinkDied");
                    sfx.play();

                    init(m_levelPath);
                }
                else
                {
                    m_player->addComponent<CInvincibility>(30);
                    sf::Sound &sfx = m_game->assets().getSound("SLinkDamaged");
                    sfx.play();
                }
            }
        }
    }
}

void Scene_Zelda::swordNPCCollisions()
{
    for (const std::shared_ptr<Entity> &sword : m_entityManager.getEntities("Sword"))
    {
        if (sword->hasComponent<CDamage>())
        {
            for (const std::shared_ptr<Entity> &enemy : m_entityManager.getEntities("NPC"))
            {
                Vec2 overlap = Physics::GetOverlap(sword, enemy);
                if (overlap.x > 0 && overlap.y > 0)
                {
                    CHealth &enemyHealth = enemy->getComponent<CHealth>();
                    CDamage &swordDamage = sword->getComponent<CDamage>();

                    enemyHealth.current -= swordDamage.damage;

                    if (enemyHealth.current <= 0)
                    {
                        sf::Sound &sfx = m_game->assets().getSound("SEnemyDied");
                        sfx.play();

                        Animation &explosion = m_game->assets().getAnimation("Explosion");
                        enemy->addComponent<CAnimation>(explosion, false);

                        enemy->removeComponent<CBoundingBox>();
                        enemy->removeComponent<CDamage>();
                        enemy->removeComponent<CHealth>();
                    }
                    else
                    {
                        sf::Sound &sfx = m_game->assets().getSound("SEnemyDamaged");
                        sfx.play();
                    }

                    sword->removeComponent<CDamage>();
                }
            }
        }
    }
}

void Scene_Zelda::entityHeartCollisions()
{
    for (const std::shared_ptr<Entity> &heart : m_entityManager.getEntities("Heart"))
    {
        if (heart->isActive())
        {
            for (const std::shared_ptr<Entity> &entity : m_entityManager.getEntities())
            {
                if (entity->tag() == "NPC" || entity->tag() == "Player")
                {
                    Vec2 overlap = Physics::GetOverlap(heart, entity);
                    if (overlap.x > 0 && overlap.y > 0)
                    {
                        CHealth &health = entity->getComponent<CHealth>();
                        health.current = health.max;

                        sf::Sound &sfx = m_game->assets().getSound("SLinkPickupHeart");
                        sfx.play();

                        heart->destroy();
                    }
                }
            }
        }
    }
}

void Scene_Zelda::blackTileCollisions()
{
    CInput &playerInput = m_player->getComponent<CInput>();
    CTransform &playerTransform = m_player->getComponent<CTransform>();

    const EntityVec &portals = m_entityManager.getEntities("Portal");

    bool playerOverlappingPortal = false;

    for (const auto &portal : portals)
    {
        Vec2 overlap = Physics::GetOverlap(m_player, portal);
        bool isOverlapping = overlap.x > 0 && overlap.y > 0;

        if (isOverlapping)
        {
            playerOverlappingPortal = true;

            if (playerInput.canTeleport)
            {

                std::shared_ptr<Entity> targetPortal;
                do
                {
                    int random = rand() % (portals.size());
                    targetPortal = portals[random];
                } while (portal == targetPortal && portals.size() > 1);

                playerTransform.pos = targetPortal->getComponent<CTransform>().pos;
                playerInput.canTeleport = false;
                break;
            }
        }
    }

    if (!playerOverlappingPortal && !playerInput.canTeleport)
    {
        playerInput.canTeleport = true;
    }
}

void Scene_Zelda::sAnimation()
{
    // Implement player facing direction animation
    CState &state = m_player->getComponent<CState>();
    CAnimation &anim = m_player->getComponent<CAnimation>();
    if (state.animation != anim.animation.getName())
    {
        Animation &newAnim = m_game->assets().getAnimation(state.animation);
        m_player->addComponent<CAnimation>(newAnim, true);
    }

    // Implement sword animation based on player facing
    // The sword should move if the player changes direction mid swing
    const EntityVec &swords = m_entityManager.getEntities("Sword");
    if (!swords.empty())
    {
        const std::shared_ptr<Entity> sword = swords.at(0);
        locateSword(m_player, sword);
    }

    // Implement destruction of entities with non-repeating finished animations
    for (const std::shared_ptr<Entity> &e : m_entityManager.getEntities())
    {
        if (e->hasComponent<CAnimation>())
        {
            CAnimation &animComponent = e->getComponent<CAnimation>();
            animComponent.animation.update();
            if (!animComponent.repeat && animComponent.animation.hasEnded())
            {
                e->destroy();
            }
        }
    }
}

void Scene_Zelda::sCamera()
{
    sf::View view = m_game->window().getView();
    Vec2 &pPos = m_player->getComponent<CTransform>().pos;

    if (m_follow)
    {
        view.setCenter(pPos.x, pPos.y);
    }
    else
    {
        sf::Vector2u windowSize = m_game->window().getSize();
        Vec2 wSize = Vec2(windowSize.x, windowSize.y);
        Vec2 room = Vec2(
            int(abs(pPos.x) / wSize.x),
            int(abs(pPos.y) / wSize.y));

        if (pPos.x < 0)
            room.x = -room.x - 1;

        if (pPos.y < 0)
            room.y = -room.y - 1;

        Vec2 newCenter;
        newCenter.x = (room.x * wSize.x) + (wSize.x / 2);
        newCenter.y = (room.y * wSize.y) + (wSize.y / 2);

        view.setCenter(newCenter.x, newCenter.y);
    }

    m_game->window().setView(view);
}

void Scene_Zelda::sRender()
{
    // Color the background darker when paused
    if (!m_paused)
    {
        m_game->window().clear(sf::Color(255, 190, 120));
    }
    else
    {
        m_game->window().clear(sf::Color(150, 120, 75));
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

            // Draw health bars for entities with CHealth component
            if (e->hasComponent<CHealth>())
            {
                CHealth &health = e->getComponent<CHealth>();
                CBoundingBox *box = e->hasComponent<CBoundingBox>() ? &e->getComponent<CBoundingBox>() : nullptr;

                float barWidth = box ? box->size.x : 50.f;
                float barHeight = 5.f;
                Vec2 barPos = Vec2(transform.pos.x - barWidth / 2,
                                   transform.pos.y - (box ? box->halfSize.y + 10 : 20));

                sf::RectangleShape bgBar(sf::Vector2f(barWidth, barHeight));
                bgBar.setPosition(barPos.x, barPos.y);
                bgBar.setFillColor(sf::Color::Black);
                m_game->window().draw(bgBar);

                float segmentWidth = barWidth / health.max;

                for (int i = 0; i < health.current; i++)
                {
                    sf::RectangleShape segment(sf::Vector2f(segmentWidth - 2, barHeight - 1));
                    segment.setPosition(barPos.x + i * segmentWidth + 0.5f, barPos.y + 0.5f);
                    segment.setFillColor(sf::Color::Red);
                    m_game->window().draw(segment);
                }
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
                Vec2 home = e->getComponent<CFollowPlayer>().home;

                sf::Vertex line[] = {
                    sf::Vertex(sf::Vector2f(playerCenter.x, playerCenter.y), sf::Color::Black),
                    sf::Vertex(sf::Vector2f(enemyCenter.x, enemyCenter.y), sf::Color::Black)};
                m_game->window().draw(line, 2, sf::Lines);

                sf::CircleShape homePoint(3.f);
                homePoint.setFillColor(sf::Color::Black);
                homePoint.setOrigin(3.f, 3.f);
                homePoint.setPosition(home.x, home.y);
                m_game->window().draw(homePoint);
            }
            else if (e->hasComponent<CPatrol>())
            {
                CPatrol &patrol = e->getComponent<CPatrol>();

                for (const Vec2 &point : patrol.positions)
                {
                    sf::CircleShape waypoint(3.f);
                    waypoint.setFillColor(sf::Color::Black);
                    waypoint.setOrigin(3.f, 3.f);
                    waypoint.setPosition(point.x, point.y);
                    m_game->window().draw(waypoint);
                }
            }
        }
    }
}