#ifndef O2JAM_CONFIG_GAME_CONFIG_HPP
#define O2JAM_CONFIG_GAME_CONFIG_HPP

#include <OTwo/Chart/Chart.hpp>

#include <Genode/System/Config.hpp>

#include <SFML/Window/Keyboard.hpp>

struct GameConfig : public Gx::Config
{
    GameConfig()
    {
        GameConfig::Reset();
    }

    void Reset() override
    {
        UseFx           = true;
        UseBGM          = true;
        UseWindowCursor = false;
        MusicVolume     = 100;
        EffectVolume    = 100;
        SevenKeyBinding = {
            {Chart::ChannelType::Note1, sf::Keyboard::Key::S},
            {Chart::ChannelType::Note2, sf::Keyboard::Key::D},
            {Chart::ChannelType::Note3, sf::Keyboard::Key::F},
            {Chart::ChannelType::Note4, sf::Keyboard::Key::Space},
            {Chart::ChannelType::Note5, sf::Keyboard::Key::J},
            {Chart::ChannelType::Note6, sf::Keyboard::Key::K},
            {Chart::ChannelType::Note7, sf::Keyboard::Key::L}
        };
        ThreeKeyBinding = {
            {Chart::ChannelType::Note1, sf::Keyboard::Key::LShift},
            {Chart::ChannelType::Note2, sf::Keyboard::Key::RShift},
            {Chart::ChannelType::Note3, sf::Keyboard::Key::Enter}
        };
    }

    bool UseFx;
    bool UseBGM;
    bool UseWindowCursor;

    unsigned int MusicVolume;
    unsigned int EffectVolume;

    std::map<Chart::ChannelType, sf::Keyboard::Key> SevenKeyBinding;
    std::map<Chart::ChannelType, sf::Keyboard::Key> ThreeKeyBinding;
};

#endif
