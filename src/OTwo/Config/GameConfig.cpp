#include <OTwo/Config/GameConfig.hpp>

#include <magic_enum/magic_enum.hpp>

#if _WIN32
#include <winreg/winreg.hpp>

enum class DirectInputKey : std::uint16_t
{
    Unknown        = 0xFFFF,
    A              = 0x1E,
    B              = 0x30,
    C              = 0x2E,
    D              = 0x20,
    E              = 0x12,
    F              = 0x21,
    G              = 0x22,
    H              = 0x23,
    I              = 0x17,
    J              = 0x24,
    K              = 0x25,
    L              = 0x26,
    M              = 0x32,
    N              = 0x31,
    O              = 0x18,
    P              = 0x19,
    Q              = 0x10,
    R              = 0x13,
    S              = 0x1F,
    T              = 0x14,
    U              = 0x16,
    V              = 0x2F,
    W              = 0x11,
    X              = 0x2D,
    Y              = 0x15,
    Z              = 0x2C,
    Num0           = 0x0B,
    Num1           = 0x02,
    Num2           = 0x03,
    Num3           = 0x04,
    Num4           = 0x05,
    Num5           = 0x06,
    Num6           = 0x07,
    Num7           = 0x08,
    Num8           = 0x09,
    Num9           = 0x0A,
    Escape         = 0x01,
    LControl       = 0x1D,
    RControl       = 0x9D,
    LShift         = 0x2A,
    RShift         = 0x36,
    LAlt           = 0x38,
    RAlt           = 0xB8,
    LSystem        = 0xDB,
    RSystem        = 0xDC,
    Menu           = 0xDD,
    LBracket       = 0x1A,
    RBracket       = 0x1B,
    Semicolon      = 0x27,
    Comma          = 0x33,
    Period         = 0x34,
    Apostrophe     = 0x28,
    Slash          = 0x35,
    Backslash      = 0x2B,
    Grave          = 0x29,
    Equal          = 0x0D,
    Hyphen         = 0x0C,
    Space          = 0x39,
    Enter          = 0x1C,
    Backspace      = 0x0E,
    Tab            = 0x0F,
    PageUp         = 0xC9,
    PageDown       = 0xD1,
    End            = 0xCF,
    Home           = 0xC7,
    Insert         = 0xD2,
    Delete         = 0xD3,
    Add            = 0x4E,
    Subtract       = 0x4A,
    Multiply       = 0x37,
    Divide         = 0xB5,
    Left           = 0xCB,
    Right          = 0xCD,
    Up             = 0xC8,
    Down           = 0xD0,
    Numpad0        = 0x52,
    Numpad1        = 0x4F,
    Numpad2        = 0x50,
    Numpad3        = 0x51,
    Numpad4        = 0x4B,
    Numpad5        = 0x4C,
    Numpad6        = 0x4D,
    Numpad7        = 0x47,
    Numpad8        = 0x48,
    Numpad9        = 0x49,
    F1             = 0x3B,
    F2             = 0x3C,
    F3             = 0x3D,
    F4             = 0x3E,
    F5             = 0x3F,
    F6             = 0x40,
    F7             = 0x41,
    F8             = 0x42,
    F9             = 0x43,
    F10            = 0x44,
    F11            = 0x57,
    F12            = 0x58,
    F13            = 0x64,
    F14            = 0x65,
    F15            = 0x66,
    Pause          = 0xC5
};

using namespace winreg;
#endif

#include <map>

GameConfig::GameConfig()
{
    Reset();
}

void GameConfig::Load()
{
    Reset();

#if _WIN32
    // Registry (Original: HKEY_LOCAL_MACHINE)
    const auto root = RegKey(HKEY_LOCAL_MACHINE);
    if (const auto eq = root.TryGetDwordValue(L"EQ"); eq.IsValid())
        UseEqualizer = eq.GetValue() == 1;

    if (const auto fx = root.TryGetDwordValue(L"3D_Mode"); fx.IsValid())
        UseFx = fx.GetValue() != 1;

    if (const auto fx = root.TryGetDwordValue(L"3DEffectLevel"); fx.IsValid())
        UseFx = (FxLevel = fx.GetValue()) != 0 || UseFx;

    if (const auto mouse = root.TryGetDwordValue(L"Mouse_Cursor"); mouse.IsValid())
        UseWindowCursor = mouse.GetValue() != 0;

    // INI (Original: C:\Windows\option.ini)
    // Virtualized Path: C:\Users\{Username}\AppData\Local\VirtualStore\Windows
    NoteShapeType   = static_cast<NoteShape>(GetPrivateProfileIntA("NOTE", "NoteType", 0, "option.ini"));
    NoteGuideLength = GetPrivateProfileIntA("NOTE", "NoteTail", 2, "option.ini");
    MusicVolume     = GetPrivateProfileIntA("Sound", "NoteBGVolume", 255, "option.ini") / 255.f * 100;
    EffectVolume    = GetPrivateProfileIntA("Sound", "NoteKeyVolume", 255, "option.ini") / 255.f * 100;

    const std::vector defaults = { DirectInputKey::S, DirectInputKey::D, DirectInputKey::F, DirectInputKey::Space, DirectInputKey::J, DirectInputKey::K, DirectInputKey::L };
    for (int i = 0; i < 7; i++)
    {
        const auto key = std::string(std::string("KEY_") + std::to_string(i + 1));
        const auto chn = static_cast<Chart::Channel>(i + 2);
        const auto val = static_cast<DirectInputKey>(GetPrivateProfileIntA("Key", key.c_str(), static_cast<int>(defaults[i]), "option.ini"));

        const auto str = std::string(magic_enum::enum_name(val));
        KeyBindings[KeyMode::Seven][chn] = magic_enum::enum_cast<sf::Keyboard::Key>(str, magic_enum::case_insensitive).value_or(sf::Keyboard::Key::Unknown);
    }
#endif
}

void GameConfig::Save()
{
#if _WIN32
    // Registry (HKEY_LOCAL_MACHINE)
    auto root = RegKey(HKEY_LOCAL_MACHINE);
    auto _    = RegResult{};

    _ = root.TrySetDwordValue(L"EQ", UseEqualizer);
    _ = root.TrySetDwordValue(L"3D_Mode", UseFx ? 3 : 1);
    _ = root.TrySetDwordValue(L"3DEffectLevel", FxLevel);
    _ = root.TrySetDwordValue(L"Mouse_Cursor", UseWindowCursor);

    // INI (C:\Windows\option.ini)
    WritePrivateProfileStringA("NOTE", "NoteType", std::to_string(static_cast<int>(NoteShapeType)).c_str(), "option.ini");
    WritePrivateProfileStringA("NOTE", "NoteTail", std::to_string(static_cast<int>(NoteGuideLength)).c_str(), "option.ini");
    WritePrivateProfileStringA("Sound", "NoteBGVolume", std::to_string(static_cast<int>((MusicVolume / 100.f) * 255.f)).c_str(), "option.ini");
    WritePrivateProfileStringA("Sound", "NoteKeyVolume", std::to_string(static_cast<int>((EffectVolume / 100.f) * 255.f)).c_str(), "option.ini");

    const std::vector defaults = { DirectInputKey::S, DirectInputKey::D, DirectInputKey::F, DirectInputKey::Space, DirectInputKey::J, DirectInputKey::K, DirectInputKey::L };
    for (int i = 0; i < 7; i++)
    {
        const auto key = std::string(std::string("KEY_") + std::to_string(i + 1));
        const auto chn = static_cast<Chart::Channel>(i + 2);

        const auto str = std::string(magic_enum::enum_name(KeyBindings[KeyMode::Seven][chn]));
        const auto val = std::to_string(static_cast<int>(magic_enum::enum_cast<DirectInputKey>(str, magic_enum::case_insensitive).value_or(DirectInputKey::Unknown)));
        WritePrivateProfileStringA("Key", key.c_str(), val.c_str(), "option.ini");
    }
#endif
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
