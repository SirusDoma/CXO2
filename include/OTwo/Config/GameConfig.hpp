#ifndef O2JAM_CONFIG_GAME_CONFIG_HPP
#define O2JAM_CONFIG_GAME_CONFIG_HPP

#include <OTwo/Chart/Chart.hpp>
#include <OTwo/Models/Room.hpp>

#include <SFML/Window/Keyboard.hpp>

struct GameConfig : Gx::Provider
{
    GameConfig()
    {
        Reset();
    }

    void Apply(const GameConfig &src)
    {
        UseFx           = src.UseFx;
        UseBGM          = src.UseBGM;
        UseWindowCursor = src.UseWindowCursor;
        MusicVolume     = src.MusicVolume;
        EffectVolume    = src.EffectVolume;
        KeyBindings     = src.KeyBindings;
    }

    void Reset()
    {
        UseFx           = true;
        UseBGM          = true;
        UseWindowCursor = false;
        MusicVolume     = 100;
        EffectVolume    = 100;
        KeyBindings     = {
            {
                KeyMode::Seven,
                {
                    {Chart::ChannelType::Note1, sf::Keyboard::Key::S},
                    {Chart::ChannelType::Note2, sf::Keyboard::Key::D},
                    {Chart::ChannelType::Note3, sf::Keyboard::Key::F},
                    {Chart::ChannelType::Note4, sf::Keyboard::Key::Space},
                    {Chart::ChannelType::Note5, sf::Keyboard::Key::J},
                    {Chart::ChannelType::Note6, sf::Keyboard::Key::K},
                    {Chart::ChannelType::Note7, sf::Keyboard::Key::L}
                }
            },
            {
                KeyMode::Five,
                {
                    {Chart::ChannelType::Note2, sf::Keyboard::Key::D},
                    {Chart::ChannelType::Note3, sf::Keyboard::Key::F},
                    {Chart::ChannelType::Note4, sf::Keyboard::Key::Space},
                    {Chart::ChannelType::Note5, sf::Keyboard::Key::J},
                    {Chart::ChannelType::Note6, sf::Keyboard::Key::K},
                }
            },
            {
                KeyMode::Three,
                {
                    {Chart::ChannelType::Note1, sf::Keyboard::Key::LShift},
                    {Chart::ChannelType::Note2, sf::Keyboard::Key::RShift},
                    {Chart::ChannelType::Note4, sf::Keyboard::Key::Enter}
                }
            }
        };
    }

    bool UseFx;
    bool UseBGM;
    bool UseWindowCursor;

    unsigned int MusicVolume;
    unsigned int EffectVolume;

    std::map<KeyMode, std::map<Chart::ChannelType, sf::Keyboard::Key>> KeyBindings;
    std::map<Chart::ChannelType, sf::Keyboard::Key> SevenKeyBinding;
    std::map<Chart::ChannelType, sf::Keyboard::Key> ThreeKeyBinding;
};

#endif
