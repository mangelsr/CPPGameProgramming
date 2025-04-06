#pragma once

#include <map>
#include <string>
#include <memory>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Animation.h"

class Assets
{
private:
    std::map<std::string, sf::Texture> textures;
    std::map<std::string, Animation> animations;
    std::map<std::string, sf::SoundBuffer> soundBuffers;
    std::map<std::string, sf::Sound> sounds;
    std::map<std::string, sf::Font> fonts;

public:
    Assets();

    void addTexture(const std::string &name, const std::string &path);
    void addAnimation(const std::string &name, const Animation &animation);
    void addSound(const std::string &name, const std::string &path);
    void addFont(const std::string &name, const std::string &path);

    sf::Texture &getTexture(const std::string &name);
    Animation &getAnimation(const std::string &name);
    sf::Sound &getSound(const std::string &name);
    sf::Font &getFont(const std::string &name);
};