#include <iostream> // For error handling
#include <fstream>

#include "GameEngine.h"
#include "Scene_Menu.h"

GameEngine::GameEngine(const std::string &path)
{
    init(path);
}

void GameEngine::init(const std::string &path)
{
    // Initialize window
    m_window.create(sf::VideoMode(1280, 768), "My Game");
    m_window.setFramerateLimit(m_framerateLimit);

    // Load assets (textures, sounds, fonts, etc.)
    std::ifstream fileIn(path + "assets.txt");
    std::string rowIdentifier;
    while (fileIn >> rowIdentifier)
    {
        if (rowIdentifier == "Texture")
        {
            std::string name, location;
            fileIn >> name >> location;
            m_assets.addTexture(name, path + location);
        }
        else if (rowIdentifier == "Animation")
        {
            std::string name, textureName;
            size_t frameCount, speed;
            fileIn >> name >> textureName >> frameCount >> speed;
            const sf::Texture &texture = m_assets.getTexture(textureName);
            m_assets.addAnimation(name, Animation(name, texture, frameCount, speed));
        }
        else if (rowIdentifier == "Font")
        {
            std::string name, location;
            fileIn >> name >> location;
            m_assets.addFont(name, path + location);
        }
    }

    std::shared_ptr<Scene> menuScene = std::make_shared<Scene_Menu>(this); // Example
    m_sceneMap["menu"] = menuScene;
    m_currentScene = "menu";
}

void GameEngine::update()
{
    if (currentScene())
    {
        currentScene()->simulate(m_simulationSpeed);
    }
}

void GameEngine::sUserInput()
{
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            quit();

        auto scene = currentScene();
        if (scene)
        {
            std::string actionType = "";
            if (event.type == sf::Event::KeyPressed)
            {
                actionType = "START";
            }
            else if (event.type == sf::Event::KeyReleased)
            {
                actionType = "END";
            }
            int keyCode = event.key.code;

            const auto &actionMap = scene->getActionMap();
            auto it = actionMap.find(keyCode);

            if (it != actionMap.end() && actionType != "")
            {
                std::string actionName = it->second;
                scene->doAction(Action(actionName, actionType));
            }

            auto mousePos = sf::Mouse::getPosition(m_window); // important note getMouse position relative to window
            Vec2 mpos(mousePos.x, mousePos.y);

            if (event.type == sf::Event::MouseButtonPressed)
            {
                switch (event.mouseButton.button)
                {
                case sf::Mouse::Left:
                    currentScene()->doAction(Action("LEFT_CLICK", "START", mpos));
                    break;
                case sf::Mouse::Middle:
                    currentScene()->doAction(Action("MIDDLE_CLICK", "START", mpos));
                    break;
                case sf::Mouse::Right:
                    currentScene()->doAction(Action("RIGHT_CLICK", "START", mpos));
                    break;
                default:
                    break;
                }
            }

            if (event.type == sf::Event::MouseButtonReleased)
            {
                switch (event.mouseButton.button)
                {
                case sf::Mouse::Left:
                    currentScene()->doAction(Action("LEFT_CLICK", "END", mpos));
                    break;
                case sf::Mouse::Middle:
                    currentScene()->doAction(Action("MIDDLE_CLICK", "END", mpos));
                    break;
                case sf::Mouse::Right:
                    currentScene()->doAction(Action("RIGHT_CLICK", "END", mpos));
                    break;
                default:
                    break;
                }
            }

            if (event.type == sf::Event::MouseMoved)
            {
                currentScene()->doAction(Action("MOUSE_MOVE", "START", Vec2(event.mouseMove.x, event.mouseMove.y)));
            }
        }
    }
}

std::shared_ptr<Scene> GameEngine::currentScene()
{
    if (m_currentScene.empty())
        return nullptr; // Handle the case where no scene is set

    auto it = m_sceneMap.find(m_currentScene);
    if (it != m_sceneMap.end())
    {
        return it->second;
    }
    else
    {
        std::cout << "Error: Scene '" << m_currentScene << "' not found." << std::endl;
        return nullptr; // Or throw an exception
    }
}

void GameEngine::changeScene(const std::string &sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene)
{
    if (endCurrentScene && currentScene())
    {
        currentScene()->onEnd();
    }

    m_sceneMap[sceneName] = scene; // Add or replace the scene
    m_currentScene = sceneName;

    if (currentScene())
    {
        currentScene()->m_game = this; // Set the game engine pointer for the new scene
    }
}

void GameEngine::quit()
{
    m_running = false;
    m_window.close();
}

void GameEngine::run()
{
    while (m_running)
    {
        sUserInput();
        update();

        if (currentScene())
        {
            currentScene()->sRender();
            window().display();
        }
    }
}

sf::RenderWindow &GameEngine::window()
{
    return m_window;
}

Assets &GameEngine::assets()
{
    return m_assets;
}

const Assets &GameEngine::assets() const
{
    return m_assets;
}

bool GameEngine::isRunning()
{
    return m_running;
}

int GameEngine::framerateLimit()
{
    return m_framerateLimit;
}