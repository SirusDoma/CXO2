#ifndef O2JAM_CONFIG_GAME_CONFIG_HPP
#define O2JAM_CONFIG_GAME_CONFIG_HPP

#include <OTwo/Chart/Chart.hpp>

#include <Genode/System/Config.hpp>

#include <SFML/Window/Keyboard.hpp>

struct GameConfig : public Gx::Config
{
    GameConfig()
    {
        Reset();
    }

    void Reset() override
    {
        Use3D           = true;
        UseBGM          = true;
        UseWindowCursor = false;
        MusicVolume     = 100;
        EffectVolume    = 100;
        SevenKeyBinding = {
            {Chart::Channel::Note1, sf::Keyboard::Key::S},
            {Chart::Channel::Note2, sf::Keyboard::Key::D},
            {Chart::Channel::Note3, sf::Keyboard::Key::F},
            {Chart::Channel::Note4, sf::Keyboard::Key::Space},
            {Chart::Channel::Note5, sf::Keyboard::Key::J},
            {Chart::Channel::Note6, sf::Keyboard::Key::K},
            {Chart::Channel::Note7, sf::Keyboard::Key::L}
        };
        ThreeKeyBinding = {
            {Chart::Channel::Note1, sf::Keyboard::Key::LShift},
            {Chart::Channel::Note2, sf::Keyboard::Key::RShift},
            {Chart::Channel::Note3, sf::Keyboard::Key::Enter}
        };
    }

    bool Use3D;
    bool UseBGM;
    bool UseWindowCursor;

    unsigned int MusicVolume;
    unsigned int EffectVolume;

    std::map<Chart::Channel, sf::Keyboard::Key> SevenKeyBinding;
    std::map<Chart::Channel, sf::Keyboard::Key> ThreeKeyBinding;
};

#endif
