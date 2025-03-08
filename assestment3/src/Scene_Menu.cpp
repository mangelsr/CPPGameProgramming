#include "Scene_Menu.h"
#include "GameEngine.h"
#include "Scene_Play.h"

Scene_Menu::Scene_Menu(GameEngine *gameEngine)
    : Scene(gameEngine)
{
    init();
}

void Scene_Menu::init()
{
    m_title = "Mega Mario"; // Set your game title here
    m_menuStrings.push_back("Start Game");
    m_menuStrings.push_back("Options");
    m_menuStrings.push_back("Credits");
    m_menuStrings.push_back("Exit");

    m_levelPaths.push_back("level1.txt"); // Add your level paths here
    m_levelPaths.push_back("level2.txt");
    m_levelPaths.push_back("level3.txt");

    m_menuText.setFont(m_game->assets().getFont("Arial")); // Assuming you have an "Arial" font loaded
    m_menuText.setCharacterSize(30);

    registerAction(sf::Keyboard::Up, "UP");
    registerAction(sf::Keyboard::Down, "DOWN");
    registerAction(sf::Keyboard::Return, "SELECT");
}

void Scene_Menu::update()
{
    // You might add some animation or transition logic here if needed
}

void Scene_Menu::onEnd()
{
    // Clean up resources or perform actions when the scene ends
}

void Scene_Menu::sDoAction(const Action &action)
{
    if (action.type() == "UP")
    {
        if (m_selectedMenuIndex > 0)
            m_selectedMenuIndex--;
    }
    else if (action.type() == "DOWN")
    {
        if (m_selectedMenuIndex < m_menuStrings.size() - 1)
            m_selectedMenuIndex++;
    }
    else if (action.type() == "SELECT")
    {
        if (m_selectedMenuIndex == m_menuStrings.size() - 1)
        {
            m_game->quit(); // Quit the game if "Exit" is selected
        }
        else
        {
            // Load the selected level
            std::string levelPath = m_levelPaths[m_selectedMenuIndex];
            if (!levelPath.empty())
            {
                // Assuming you have a Scene_Play class to handle gameplay
                // Replace with your actual scene/level loading logic
                m_game->changeScene("play", std::make_shared<Scene_Play>(m_game, levelPath), true);
            }
        }
    }
}

void Scene_Menu::sRender()
{
    m_game->window().clear(sf::Color::Black);

    // Draw title
    m_menuText.setString(m_title);
    m_menuText.setPosition(m_game->window().getSize().x / 2.0f - m_menuText.getLocalBounds().width / 2.0f, 100);
    m_game->window().draw(m_menuText);

    // Draw menu items
    for (size_t i = 0; i < m_menuStrings.size(); ++i)
    {
        m_menuText.setString(m_menuStrings[i]);
        m_menuText.setPosition(m_game->window().getSize().x / 2.0f - m_menuText.getLocalBounds().width / 2.0f, 200 + i * 50);
        if (i == m_selectedMenuIndex)
            m_menuText.setFillColor(sf::Color::Yellow); // Highlight selected item
        else
            m_menuText.setFillColor(sf::Color::White);
        m_game->window().draw(m_menuText);
    }
}