#include <CXO2/Config/GameConfig.hpp>
#include <CXO2/Config/GameConfigImpl.hpp>

namespace Cx
{
    GameConfig::GameConfig()
    {
        Reset();
    }

    void GameConfig::Load()
    {
        Reset();
        GameConfigImpl::Load(*this);
    }

    void GameConfig::Save()
    {
        GameConfigImpl::Save(*this);
    }

    void GameConfig::Apply(const GameConfig& src)
    {
        UseFx           = src.UseFx;
        UseBGM          = src.UseBGM;
        UseEqualizer    = src.UseEqualizer;
        UseWindowCursor = src.UseWindowCursor;
        UseVsync        = src.UseVsync;
        FxLevel         = src.FxLevel;
        MusicVolume     = src.MusicVolume;
        EffectVolume    = src.EffectVolume;
        NoteShapeType   = src.NoteShapeType;
        NoteGuideLength = src.NoteGuideLength;
        PollingRate     = src.PollingRate;
        KeyBindings     = src.KeyBindings;

        Save();
    }

    void GameConfig::Reset()
    {
        UseFx           = true;
        UseBGM          = true;
        UseEqualizer    = false;
        UseWindowCursor = false;
        UseVsync        = true;
        FxLevel         = 2;
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
}
