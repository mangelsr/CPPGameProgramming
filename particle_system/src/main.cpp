#include <iostream>
#include <SFML/Window.hpp>
#include "ParticleSystem.hpp"

int main()
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Particle System");
    window.setFramerateLimit(60);

    ParticleSystem particles;
    particles.init(window.getSize());

    while (window.isOpen())
    {
        // Handle User Input
        sf::Event e;
        while (window.pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
            {
                window.close();
            }

            if (e.type == sf::Event::KeyPressed)
            {
                particles.resetParticles();
            }
        }

        // Update the particle system
        particles.update();

        // Clear the window
        window.clear(sf::Color::Black);

        // Draw the particles
        particles.draw(window);

        window.display();
    }

    return 0;
}