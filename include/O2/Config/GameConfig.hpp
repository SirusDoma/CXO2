#ifndef O2JAM_CONFIG_GAME_CONFIG_HPP
#define O2JAM_CONFIG_GAME_CONFIG_HPP

#include <Genode/System/Config.hpp>

#include <O2/Chart/EventChannel.hpp>
#include <SFML/Window/Keyboard.hpp>

struct GameConfig : public Gx::Config
{
    GameConfig()
    {
        Reset();
    }

    virtual void Reset()
    {
        Use3D           = true;
        UseBGM          = true;
        UseWindowCursor = false;
        MusicVolume     = 100;
        EffectVolume    = 100;
        SevenKeyBinding = {
            {EventChannel::Note1, sf::Keyboard::Key::S},
            {EventChannel::Note2, sf::Keyboard::Key::D},
            {EventChannel::Note3, sf::Keyboard::Key::F},
            {EventChannel::Note4, sf::Keyboard::Key::Space},
            {EventChannel::Note5, sf::Keyboard::Key::J},
            {EventChannel::Note6, sf::Keyboard::Key::K},
            {EventChannel::Note7, sf::Keyboard::Key::L}
        };
        ThreeKeyBinding = {
            {EventChannel::Note1, sf::Keyboard::Key::LShift},
            {EventChannel::Note2, sf::Keyboard::Key::RShift},
            {EventChannel::Note3, sf::Keyboard::Key::Enter}
        };
    }

    bool Use3D;
    bool UseBGM;
    bool UseWindowCursor;

    unsigned int MusicVolume;
    unsigned int EffectVolume;

    std::map<EventChannel, sf::Keyboard::Key> SevenKeyBinding;
    std::map<EventChannel, sf::Keyboard::Key> ThreeKeyBinding;
};

#endif
