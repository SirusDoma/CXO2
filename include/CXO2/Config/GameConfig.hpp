#pragma once

#include <CXO2/Core/Chart.hpp>
#include <CXO2/Models/Room.hpp>

#include <SFML/Window/Keyboard.hpp>

#include <magic_enum/magic_enum.hpp>
#include <map>

namespace Cx
{
    enum class NoteShape : std::uint8_t
    {
        Circle = 0,
        Square = 1
    };

    struct GameConfig
    {
        GameConfig();

        void Load();
        void Save();

        void Apply(const GameConfig& src);
        void Reset();

        bool UseFx;
        bool UseBGM;
        bool UseEqualizer;
        bool UseWindowCursor;
        bool UseVsync;

        unsigned int FxLevel;
        unsigned int MusicVolume;
        unsigned int EffectVolume;

        NoteShape NoteShapeType;
        unsigned int NoteGuideLength;

        unsigned int PollingRate;

        std::map<KeyMode, std::map<Chart::Channel, sf::Keyboard::Key>> KeyBindings;
    };
}
