#include <iostream>
#include <memory>
#include <SFML/Graphics.hpp>

int main()
{
    const int wWidth = 800;
    const int wHeight = 600;

    sf::RenderWindow window(sf::VideoMode(wWidth, wHeight), "SFML works!");
    window.setFramerateLimit(60);

    sf::CircleShape circle(50);
    circle.setFillColor(sf::Color(255, 100, 100));
    circle.setPosition(100, 150);
    float circleMoveSpeed = 0.5f;

    sf::Vector2f rSize(300, 100);
    sf::RectangleShape rect(rSize);
    rect.setPosition(100, 5);
    rect.setFillColor(sf::Color(255, 255, 0));
    rect.setOutlineColor(sf::Color::Red);
    rect.setOutlineThickness(20);

    // std::vector<sf::RectangleShape> rectangles;

    // for (int x = 0; x < 25; x++)
    // {
    //     for (int y = 0; y < 15; y++)
    //     {
    //         sf::RectangleShape rect(sf::Vector2f(15, 15));
    //         rect.setPosition(x * 20, y * 20);
    //         rect.setFillColor(sf::Color(x * 10, y * 10, 0));
    //         rectangles.push_back(rect);
    //     }
    // }

    
    // Most general way to use safe pointers and store all shapes in a vector (stack)
    // This approach needs to use #include <memory>
    // all instantiated public props and mVariables need to be accessed with ->
    std::vector<std::shared_ptr<sf::Shape>> shapes;

    std::shared_ptr<sf::Shape> circleShape = std::make_shared<sf::CircleShape>(20);
    circleShape->setPosition(50, 50);
    circleShape->setFillColor(sf::Color::Green);

    std::shared_ptr<sf::Shape> rectShape = std::make_shared<sf::RectangleShape>(sf::Vector2f(100, 100));
    rectShape->setPosition(300, 300);

    shapes.push_back(circleShape);
    shapes.push_back(rectShape);

    sf::Font myFont;
    if (!myFont.loadFromFile("fonts/Silver.ttf"))
    {
        std::cerr << "Could not load font!";
        exit(-1);
    }

    sf::Text text("Sample Text", myFont, 48); // Font size in pixels not points
    text.setPosition(0, wHeight - (float)text.getCharacterSize() - 10);

    // Main Loop - continue for each frame while window is open
    while (window.isOpen())
    {
        // event handling
        sf::Event event;

        while (window.pollEvent(event))
        {
            // this event triggers when the window is closed
            if (event.type == sf::Event::Closed)
                window.close();

            // this event is triggered when a key is pressed
            if (event.type == sf::Event::KeyPressed)
            {
                // print the key that was pressed to the console
                std::cout << "Key pressed with code = " << event.key.code << "\n";

                // example, what happens when x is pressed
                if (event.key.code == sf::Keyboard::X)
                {
                    // reverse the direction of the circle on the screen
                    circleMoveSpeed *= -1;
                }
            }
        }

        // basic animation - move the figure each frame if it's still in frame
        circle.setPosition(circle.getPosition().x + circleMoveSpeed,
                           circle.getPosition().y + circleMoveSpeed);

        rect.rotate(0.1);

        // basic rendering function calls
        window.clear(); // clear the window of anything previously drawn
        // window.draw(circle); // draw the circle
        window.draw(text); // draw the text
        // window.draw(rect);   // draw the rectangle

        // for (auto &rect : rectangles)
        // {
        //     window.draw(rect);
        // }

        for (auto &shape : shapes)
        {
            window.draw(*shape);
        }

        window.display(); // call the window display function
        // Display method swap the OpenGL Buffers it uses to render on the screen
    }
    return 0;
}
