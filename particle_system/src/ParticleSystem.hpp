#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

class ParticleSystem
{
    struct Particle
    {
        sf::Vector2f velocity;
        int lifetime = 0;
    };

    std::vector<Particle> m_particles;
    sf::VertexArray m_vertices;
    sf::Vector2u m_windowSize;
    float m_size = 4;

    sf::Vector2f m_emitter;

    void resetParticle(std::size_t index, bool first = false)
    {
        // Give the particle an initial position
        float mx = m_windowSize.x / 2;
        float my = m_windowSize.y / 2;

        m_vertices[4 * index + 0].position = sf::Vector2f(mx, my);
        m_vertices[4 * index + 1].position = sf::Vector2f(mx + m_size, my);
        m_vertices[4 * index + 2].position = sf::Vector2f(mx + m_size, my + m_size);
        m_vertices[4 * index + 3].position = sf::Vector2f(mx, my + m_size);

        // Give the particle a color
        sf::Color color(128 + rand() % 128, 55, 55, rand() % 255);

        // This check is for managing the initial load
        // We hide it
        if (first)
            color.a = 0;

        m_vertices[4 * index + 0].color = color;
        m_vertices[4 * index + 1].color = color;
        m_vertices[4 * index + 2].color = color;
        m_vertices[4 * index + 3].color = color;

        // Give the particle a random velocity
        // Random float from -5 to +5
        float rx = (float)rand() / RAND_MAX * 10 - 5;
        float ry = (float)rand() / RAND_MAX * 10 - 5;

        m_particles[index].velocity = sf::Vector2f(rx, ry);

        // Give the particle a lifespan
        // Random integer from 30 to 90
        m_particles[index].lifetime = 30 + rand() % 60;
    }

public:
    void init(sf::Vector2u windowSize)
    {
        m_windowSize = windowSize;
        resetParticles();
    }

    void resetParticles(size_t count = 1024, float size = 4)
    {
        m_particles = std::vector<Particle>(count);
        m_vertices = sf::VertexArray(sf::Quads, count * 4);
        m_size = size;

        for (size_t p = 0; p < m_particles.size(); p++)
        {
            resetParticle(p, true);
        }
    }

    void update()
    {
        for (std::size_t i = 0; i < m_particles.size(); ++i)
        {
            if (m_particles[i].lifetime == 0)
            {
                resetParticle(i);
            }

            m_vertices[4 * i + 0].position += m_particles[i].velocity;
            m_vertices[4 * i + 1].position += m_particles[i].velocity;
            m_vertices[4 * i + 2].position += m_particles[i].velocity;
            m_vertices[4 * i + 3].position += m_particles[i].velocity;

            m_particles[i].lifetime--;
        }
    }

    void draw(sf::RenderWindow &window) const
    {
        window.draw(m_vertices);
    }
};