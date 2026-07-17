#include <CXO2/Config/Impl/Win32/GameConfigImpl.hpp>

#include <magic_enum/magic_enum.hpp>

#include <optional>

#include <windows.h>

namespace Cx
{
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

    namespace
    {
        std::optional<DWORD> TryGetDwordValue(const wchar_t* name)
        {
            DWORD value = 0;
            DWORD size  = sizeof(value);
            DWORD type  = 0;
            if (RegQueryValueExW(HKEY_LOCAL_MACHINE, name, nullptr, &type, reinterpret_cast<LPBYTE>(&value), &size) != ERROR_SUCCESS)
                return std::nullopt;

            if (type != REG_DWORD)
                return std::nullopt;

            return value;
        }

        void TrySetDwordValue(const wchar_t* name, const DWORD value)
        {
            RegSetValueExW(HKEY_LOCAL_MACHINE, name, 0, REG_DWORD, reinterpret_cast<const BYTE*>(&value), sizeof(value));
        }
    }

    void GameConfigImpl::Load(GameConfig& config)
    {
        // Registry (Original: HKEY_LOCAL_MACHINE)
        if (const auto eq = TryGetDwordValue(L"EQ"))
            config.UseEqualizer = *eq == 1;

        if (const auto fx = TryGetDwordValue(L"3D_Mode"))
            config.UseFx = *fx != 1;

        if (const auto fx = TryGetDwordValue(L"3DEffectLevel"))
            config.UseFx = (config.FxLevel = *fx) != 0 || config.UseFx;

        if (const auto mouse = TryGetDwordValue(L"Mouse_Cursor"))
            config.UseWindowCursor = *mouse != 0;

        // INI (Original: C:\Windows\option.ini)
        // Virtualized Path: C:\Users\{Username}\AppData\Local\VirtualStore\Windows
        config.NoteShapeType   = static_cast<NoteShape>(GetPrivateProfileIntA("NOTE", "NoteType", 0, "option.ini"));
        config.NoteGuideLength = GetPrivateProfileIntA("NOTE", "NoteTail", 2, "option.ini");
        config.MusicVolume     = GetPrivateProfileIntA("Sound", "NoteBGVolume", 255, "option.ini") / 255.f * 100;
        config.EffectVolume    = GetPrivateProfileIntA("Sound", "NoteKeyVolume", 255, "option.ini") / 255.f * 100;

        const std::vector defaults = { DirectInputKey::S, DirectInputKey::D, DirectInputKey::F, DirectInputKey::Space, DirectInputKey::J, DirectInputKey::K, DirectInputKey::L };
        for (int i = 0; i < 7; i++)
        {
            const auto key = std::string(std::string("KEY_") + std::to_string(i + 1));
            const auto chn = static_cast<Chart::Channel>(i + 2);
            const auto val = static_cast<DirectInputKey>(GetPrivateProfileIntA("Key", key.c_str(), static_cast<int>(defaults[i]), "option.ini"));

            const auto str = std::string(magic_enum::enum_name(val));
            config.KeyBindings[KeyMode::Seven][chn] = magic_enum::enum_cast<sf::Keyboard::Key>(str, magic_enum::case_insensitive).value_or(sf::Keyboard::Key::Unknown);
        }
    }

    void GameConfigImpl::Save(const GameConfig& config)
    {
        // Registry (HKEY_LOCAL_MACHINE)
        TrySetDwordValue(L"EQ", config.UseEqualizer);
        TrySetDwordValue(L"3D_Mode", config.UseFx ? 3 : 1);
        TrySetDwordValue(L"3DEffectLevel", config.FxLevel);
        TrySetDwordValue(L"Mouse_Cursor", config.UseWindowCursor);

        // INI (C:\Windows\option.ini)
        WritePrivateProfileStringA("NOTE", "NoteType", std::to_string(static_cast<int>(config.NoteShapeType)).c_str(), "option.ini");
        WritePrivateProfileStringA("NOTE", "NoteTail", std::to_string(static_cast<int>(config.NoteGuideLength)).c_str(), "option.ini");
        WritePrivateProfileStringA("Sound", "NoteBGVolume", std::to_string(static_cast<int>((config.MusicVolume / 100.f) * 255.f)).c_str(), "option.ini");
        WritePrivateProfileStringA("Sound", "NoteKeyVolume", std::to_string(static_cast<int>((config.EffectVolume / 100.f) * 255.f)).c_str(), "option.ini");

        const std::vector defaults = { DirectInputKey::S, DirectInputKey::D, DirectInputKey::F, DirectInputKey::Space, DirectInputKey::J, DirectInputKey::K, DirectInputKey::L };
        for (int i = 0; i < 7; i++)
        {
            const auto key = std::string(std::string("KEY_") + std::to_string(i + 1));
            const auto chn = static_cast<Chart::Channel>(i + 2);

            const auto str = std::string(magic_enum::enum_name(config.KeyBindings.at(KeyMode::Seven).at(chn)));
            const auto val = std::to_string(static_cast<int>(magic_enum::enum_cast<DirectInputKey>(str, magic_enum::case_insensitive).value_or(DirectInputKey::Unknown)));
            WritePrivateProfileStringA("Key", key.c_str(), val.c_str(), "option.ini");
        }
    }
}
