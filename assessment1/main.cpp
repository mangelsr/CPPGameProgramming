#include <fstream>
#include <iostream>
#include <memory>
#include <SFML/Graphics.hpp>

int main()
{
    std::ifstream fileIn("config.txt");

    std::string rowIdentifier;

    int wWidth, wHeight;

    std::string fontFile;
    int fontSize, fontColorR, fontColorG, fontColorB;

    std::vector<std::shared_ptr<sf::Shape>> shapes;

    while (fileIn >> rowIdentifier)
    {
        if (rowIdentifier == "Window")
        {
            fileIn >> wWidth >> wHeight;
        }
        else if (rowIdentifier == "Font")
        {
            fileIn >> fontFile >> fontSize >> fontColorR >> fontColorG >> fontColorB;
        }
        else if (rowIdentifier == "Circle")
        {
            std::string label;
            int positionX, positionY;
            float velocityX, velocityY;
            int colorR, colorG, colorB;
            int radius;

            fileIn >> label;
            fileIn >> positionX >> positionY;
            fileIn >> velocityX >> velocityY;
            fileIn >> colorR >> colorG >> colorB;
            fileIn >> radius;

            std::shared_ptr<sf::Shape> circle = std::make_shared<sf::CircleShape>(radius);
            circle->setPosition(positionX, positionY);
            circle->setFillColor(sf::Color(colorR, colorG, colorB));

            shapes.push_back(circle);
        }
        else if (rowIdentifier == "Rectangle")
        {
            std::string label;
            int positionX, positionY;
            float velocityX, velocityY;
            int colorR, colorG, colorB;
            int width, height;

            fileIn >> label;
            fileIn >> positionX >> positionY;
            fileIn >> velocityX >> velocityY;
            fileIn >> colorR >> colorG >> colorB;
            fileIn >> width >> height;

            std::shared_ptr<sf::Shape> rect = std::make_shared<sf::RectangleShape>(sf::Vector2f(width, height));
            rect->setPosition(positionX, positionY);
            rect->setFillColor(sf::Color(colorR, colorG, colorB));

            shapes.push_back(rect);
        }
        else
        {
            std::cout << "This row can't be parsed";
            exit(-1);
        }
    }

    sf::RenderWindow window(sf::VideoMode(wWidth, wHeight), "SFML works!");
    window.setFramerateLimit(60);

    sf::Font myFont;
    if (!myFont.loadFromFile(fontFile))
    {
        std::cerr << "Could not load font!";
        exit(-1);
    }

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();

        for (auto &shape : shapes)
        {
            window.draw(*shape);
        }

        window.display();
    }
    return 0;
}
