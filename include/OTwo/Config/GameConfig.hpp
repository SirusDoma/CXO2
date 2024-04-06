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
            {O2Chart::Channel::Note1, sf::Keyboard::Key::S},
            {O2Chart::Channel::Note2, sf::Keyboard::Key::D},
            {O2Chart::Channel::Note3, sf::Keyboard::Key::F},
            {O2Chart::Channel::Note4, sf::Keyboard::Key::Space},
            {O2Chart::Channel::Note5, sf::Keyboard::Key::J},
            {O2Chart::Channel::Note6, sf::Keyboard::Key::K},
            {O2Chart::Channel::Note7, sf::Keyboard::Key::L}
        };
        ThreeKeyBinding = {
            {O2Chart::Channel::Note1, sf::Keyboard::Key::LShift},
            {O2Chart::Channel::Note2, sf::Keyboard::Key::RShift},
            {O2Chart::Channel::Note3, sf::Keyboard::Key::Enter}
        };
    }

    bool Use3D;
    bool UseBGM;
    bool UseWindowCursor;

    unsigned int MusicVolume;
    unsigned int EffectVolume;

    std::map<O2Chart::Channel, sf::Keyboard::Key> SevenKeyBinding;
    std::map<O2Chart::Channel, sf::Keyboard::Key> ThreeKeyBinding;
};

#endif
