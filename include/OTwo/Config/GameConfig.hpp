#ifndef O2JAM_CONFIG_GAME_CONFIG_HPP
#define O2JAM_CONFIG_GAME_CONFIG_HPP

#include <OTwo/Core/Chart.hpp>
#include <OTwo/Models/Room.hpp>

#include <SFML/Window/Keyboard.hpp>

#include <map>

enum class NoteShape : Gx::Uint8
{
    Square = 1,
    Circle = 2
};

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
        NoteShapeType   = src.NoteShapeType;
        NoteGuideLength = src.NoteGuideLength;
        PollingRate     = src.PollingRate;
        KeyBindings     = src.KeyBindings;
    }

    void Reset()
    {
        UseFx           = true;
        UseBGM          = true;
        UseWindowCursor = false;
        MusicVolume     = 100;
        EffectVolume    = 100;
        NoteShapeType   = NoteShape::Square,
        NoteGuideLength = 0,
        PollingRate     = 1000; // hz
        KeyBindings     = {
            {
                KeyMode::Seven,
                {
                    {Chart::Channel::Note1, sf::Keyboard::Key::S},
                    {Chart::Channel::Note2, sf::Keyboard::Key::D},
                    {Chart::Channel::Note3, sf::Keyboard::Key::F},
                    {Chart::Channel::Note4, sf::Keyboard::Key::Space},
                    {Chart::Channel::Note5, sf::Keyboard::Key::J},
                    {Chart::Channel::Note6, sf::Keyboard::Key::K},
                    {Chart::Channel::Note7, sf::Keyboard::Key::L}
                }
            },
            {
                KeyMode::Five,
                {
                    {Chart::Channel::Note2, sf::Keyboard::Key::D},
                    {Chart::Channel::Note3, sf::Keyboard::Key::F},
                    {Chart::Channel::Note4, sf::Keyboard::Key::Space},
                    {Chart::Channel::Note5, sf::Keyboard::Key::J},
                    {Chart::Channel::Note6, sf::Keyboard::Key::K},
                }
            },
            {
                KeyMode::Three,
                {
                    {Chart::Channel::Note1, sf::Keyboard::Key::LShift},
                    {Chart::Channel::Note2, sf::Keyboard::Key::RShift},
                    {Chart::Channel::Note4, sf::Keyboard::Key::Enter}
                }
            }
        };
    }

    bool UseFx;
    bool UseBGM;
    bool UseWindowCursor;

    unsigned int MusicVolume;
    unsigned int EffectVolume;

    NoteShape NoteShapeType;
    unsigned int NoteGuideLength;

    unsigned int PollingRate;

    std::map<KeyMode, std::map<Chart::Channel, sf::Keyboard::Key>> KeyBindings;
};

#endif
