#include <fstream>
#include <iostream>
#include <memory>
#include <SFML/Graphics.hpp>

class CustomShape
{
    float mVelocityX, mVelocityY;
    std::shared_ptr<sf::Shape> mShape;
    std::shared_ptr<sf::Text> mShapeLabel;

public:
    CustomShape() {}

    CustomShape(float velocityX, float velocityY,
                std::shared_ptr<sf::Shape> shape, std::shared_ptr<sf::Text> shapeLabel)

        : mVelocityX(velocityX),
          mVelocityY(velocityY),
          mShape(shape),
          mShapeLabel(shapeLabel)
    {
        sf::Vector2f shapePostiion = shape->getPosition();
        sf::FloatRect shapeRect = mShape->getLocalBounds();
        sf::FloatRect labelRect = mShapeLabel->getLocalBounds();

        mShapeLabel->setPosition(shapePostiion.x + shapeRect.width / 2 - labelRect.width / 2,
                                 shapePostiion.y + shapeRect.height / 2 - labelRect.height / 2);
    }

    void move()
    {
        sf::Vector2f currentShapePosition = mShape->getPosition();

        mShape->setPosition(currentShapePosition.x + mVelocityX,
                            currentShapePosition.y + mVelocityY);

        sf::Vector2f currentShapeLabelPosition = mShapeLabel->getPosition();

        mShapeLabel->setPosition(currentShapeLabelPosition.x + mVelocityX,
                                 currentShapeLabelPosition.y + mVelocityY);
    }

    void draw(sf::RenderWindow &window)
    {
        move();
        window.draw(*mShape);
        window.draw(*mShapeLabel);
    }
};

int main()
{
    std::ifstream fileIn("config.txt");

    std::string rowIdentifier;

    int wWidth, wHeight;

    std::string fontFile;
    int fontSize, fontColorR, fontColorG, fontColorB;

    fileIn >> rowIdentifier;

    if (rowIdentifier == "Window")
    {
        fileIn >> wWidth >> wHeight;
    }
    else
    {
        std::cout << "Error reading the config file, Must start with Window config" << "\n";
        exit(-1);
    }

    sf::RenderWindow window(sf::VideoMode(wWidth, wHeight), "SFML works!");
    window.setFramerateLimit(60);

    fileIn >> rowIdentifier;

    if (rowIdentifier == "Font")
    {
        fileIn >> fontFile >> fontSize >> fontColorR >> fontColorG >> fontColorB;
    }
    else
    {
        std::cout << "Error reading the config file, Second line should be Font config" << "\n";
        exit(-1);
    }

    sf::Font myFont;
    if (!myFont.loadFromFile(fontFile))
    {
        std::cerr << "Could not load font!";
        exit(-1);
    }

    std::vector<CustomShape> shapes;

    while (fileIn >> rowIdentifier)
    {
        if (rowIdentifier == "Circle" || rowIdentifier == "Rectangle")
        {
            std::string label;
            int positionX, positionY;
            float velocityX, velocityY;
            int colorR, colorG, colorB;

            fileIn >> label;
            fileIn >> positionX >> positionY;
            fileIn >> velocityX >> velocityY;
            fileIn >> colorR >> colorG >> colorB;

            std::shared_ptr<sf::Text> text = std::make_shared<sf::Text>(label, myFont, fontSize);
            text->setFillColor(sf::Color(fontColorR, fontColorG, fontColorB));

            if (rowIdentifier == "Circle")
            {
                int radius;
                fileIn >> radius;

                std::shared_ptr<sf::Shape> circle = std::make_shared<sf::CircleShape>(radius);
                circle->setPosition(positionX, positionY);
                circle->setFillColor(sf::Color(colorR, colorG, colorB));

                shapes.push_back(CustomShape(velocityX, velocityY, circle, text));
            }
            else if (rowIdentifier == "Rectangle")
            {
                int width, height;
                fileIn >> width >> height;

                std::shared_ptr<sf::Shape> rect = std::make_shared<sf::RectangleShape>(sf::Vector2f(width, height));
                rect->setPosition(positionX, positionY);
                rect->setFillColor(sf::Color(colorR, colorG, colorB));

                shapes.push_back(CustomShape(velocityX, velocityY, rect, text));
            }
        }
        else
        {
            std::cout << "This row can't be parsed";
            exit(-1);
        }
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
            shape.draw(window);
        }

        window.display();
    }
    return 0;
}
