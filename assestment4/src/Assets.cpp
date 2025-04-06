#include <iostream> // For error handling (std::cerr)

#include "Assets.h"

Assets::Assets() {}

void Assets::addTexture(const std::string &name, const std::string &path)
{
    sf::Texture texture;
    if (!texture.loadFromFile(path))
    {
        std::cerr << "Error loading texture: " << path << std::endl;
        return; // Or throw an exception if you prefer
    }
    textures[name] = texture;
}

void Assets::addAnimation(const std::string &name, const Animation &animation)
{
    animations[name] = animation;
}

void Assets::addSound(const std::string &name, const std::string &path)
{
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(path))
    {
        std::cerr << "Error loading sound buffer: " << path << std::endl;
        return;
    }

    soundBuffers[name] = std::move(buffer);

    sounds[name] = sf::Sound();
    sounds[name].setBuffer(soundBuffers[name]);
}

void Assets::addFont(const std::string &name, const std::string &path)
{
    sf::Font font;
    if (!font.loadFromFile(path))
    {
        std::cerr << "Error loading font: " << path << std::endl;
        return;
    }
    fonts[name] = font;
}

sf::Texture &Assets::getTexture(const std::string &name)
{
    try
    {
        return textures.at(name); // Use .at() for bounds checking and exception if not found
    }
    catch (const std::out_of_range &oor)
    {
        std::cerr << "Error: Texture '" << name << "' not found. " << oor.what() << std::endl;
        // Handle the error appropriately (e.g., return a default texture, throw an exception)
        static sf::Texture defaultTexture; // Static to avoid repeated construction
        return defaultTexture;             // Return a default texture (or throw)
    }
}

Animation &Assets::getAnimation(const std::string &name)
{
    try
    {
        return animations.at(name);
    }
    catch (const std::out_of_range &oor)
    {
        std::cerr << "Error: Animation '" << name << "' not found. " << oor.what() << std::endl;
        static Animation defaultAnimation;
        return defaultAnimation;
    }
}

sf::Sound &Assets::getSound(const std::string &name)
{
    try
    {
        return sounds.at(name);
    }
    catch (const std::out_of_range &oor)
    {
        std::cerr << "Error: Sound '" << name << "' not found. " << oor.what() << std::endl;
        static sf::Sound defaultSound;
        return defaultSound;
    }
}

sf::Font &Assets::getFont(const std::string &name)
{
    try
    {
        return fonts.at(name);
    }
    catch (const std::out_of_range &oor)
    {
        std::cerr << "Error: Font '" << name << "' not found. " << oor.what() << std::endl;
        static sf::Font defaultFont;
        return defaultFont;
    }
}