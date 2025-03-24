#include "GameEngine.h"
#include "Scene_Menu.h"
#include "Scene_Play.h"

Scene_Menu::Scene_Menu(GameEngine *gameEngine)
    : Scene(gameEngine)
{
    init();
}

void Scene_Menu::init()
{
    m_title = "MEGA MARIO";
    m_menuStrings.push_back("LEVEL 1");
    m_menuStrings.push_back("LEVEL 2");
    m_menuStrings.push_back("LEVEL 3");

    m_levelPaths.push_back("./assets/level1.txt");
    m_levelPaths.push_back("./assets/level2.txt");
    m_levelPaths.push_back("./assets/level3.txt");

    m_menuText.setFont(m_game->assets().getFont("Silver"));
    m_menuText.setCharacterSize(70);
    m_menuText.setFillColor(sf::Color::Black);

    m_helpText.setFont(m_game->assets().getFont("Silver"));
    m_helpText.setCharacterSize(45);
    m_helpText.setFillColor(sf::Color::Black);
    m_helpText.setString("UP: W    DOWN: S    PLAY: D    BACK: ESC");

    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::D, "SELECT");
    registerAction(sf::Keyboard::Escape, "BACK");
}

void Scene_Menu::update()
{
    // Puedes agregar lógica de animación o transición aquí si es necesario
}

void Scene_Menu::onEnd()
{
    // Limpiar recursos o realizar acciones cuando la escena termina
}

void Scene_Menu::sDoAction(const Action &action)
{
    if (action.type() == "START")
    {
        if (action.name() == "UP")
        {
            if (m_selectedMenuIndex > 0)
                m_selectedMenuIndex--;
        }
        else if (action.name() == "DOWN")
        {
            if (m_selectedMenuIndex < m_menuStrings.size() - 1)
                m_selectedMenuIndex++;
        }
        else if (action.name() == "SELECT")
        {
            std::string levelPath = m_levelPaths[m_selectedMenuIndex];
            if (!levelPath.empty())
            {
                m_game->changeScene("play", std::make_shared<Scene_Play>(m_game, levelPath), true);
            }
        }
        else if (action.name() == "BACK")
        {
            m_game->quit();
        }
    }
}

void Scene_Menu::sRender()
{
    m_game->window().clear(sf::Color(100, 100, 255));

    m_menuText.setString(m_title);
    m_menuText.setPosition(20, 20);
    m_menuText.setFillColor(sf::Color::Black);
    m_game->window().draw(m_menuText);

    for (size_t i = 0; i < m_menuStrings.size(); i++)
    {
        m_menuText.setString(m_menuStrings[i]);
        m_menuText.setPosition(40, 90 + i * 65);
        if (i == m_selectedMenuIndex)
            m_menuText.setFillColor(sf::Color::White);
        else
            m_menuText.setFillColor(sf::Color::Black);
        m_game->window().draw(m_menuText);
    }

    m_helpText.setPosition(20, m_game->window().getSize().y - 65);
    m_game->window().draw(m_helpText);
}