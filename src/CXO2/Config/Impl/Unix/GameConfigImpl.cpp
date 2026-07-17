#include <CXO2/Config/Impl/Unix/GameConfigImpl.hpp>

#include <magic_enum/magic_enum.hpp>
#include <nlohmann/json.hpp>

#include <cstdlib>
#include <filesystem>
#include <fstream>

namespace Cx
{
    namespace
    {
        std::filesystem::path GetConfigPath()
        {
            const char* home = std::getenv("HOME");

#if defined(__APPLE__)
            const auto base = std::filesystem::path(home ? home : ".") / "Library" / "Application Support";
#else
            std::filesystem::path base;
            if (const char* xdg = std::getenv("XDG_CONFIG_HOME"); xdg && *xdg)
                base = xdg;
            else
                base = std::filesystem::path(home ? home : ".") / ".config";
#endif

            return base / "CXO2" / "config.json";
        }
    }

    void GameConfigImpl::Load(GameConfig& config)
    {
        std::ifstream file(GetConfigPath());
        if (!file)
            return;

        const auto json = nlohmann::json::parse(file, nullptr, false);
        if (json.is_discarded() || !json.is_object())
            return;

        config.UseEqualizer    = json.value("UseEqualizer", config.UseEqualizer);
        config.UseFx           = json.value("UseFx", config.UseFx);
        config.FxLevel         = json.value("FxLevel", config.FxLevel);
        config.UseWindowCursor = json.value("UseWindowCursor", config.UseWindowCursor);
        config.NoteGuideLength = json.value("NoteGuideLength", config.NoteGuideLength);
        config.MusicVolume     = json.value("MusicVolume", config.MusicVolume);
        config.EffectVolume    = json.value("EffectVolume", config.EffectVolume);

        const auto shape = json.value("NoteShapeType", std::string());
        config.NoteShapeType = magic_enum::enum_cast<NoteShape>(shape, magic_enum::case_insensitive).value_or(config.NoteShapeType);

        if (const auto bindings = json.find("KeyBindings"); bindings != json.end() && bindings->is_object())
        {
            for (const auto& [channel, key] : bindings->items())
            {
                const auto chn = magic_enum::enum_cast<Chart::Channel>(channel, magic_enum::case_insensitive);
                const auto val = magic_enum::enum_cast<sf::Keyboard::Key>(key.get<std::string>(), magic_enum::case_insensitive);
                if (chn && val)
                    config.KeyBindings[KeyMode::Seven][*chn] = *val;
            }
        }
    }

    void GameConfigImpl::Save(const GameConfig& config)
    {
        nlohmann::json json;
        json["UseEqualizer"]    = config.UseEqualizer;
        json["UseFx"]           = config.UseFx;
        json["FxLevel"]         = config.FxLevel;
        json["UseWindowCursor"] = config.UseWindowCursor;
        json["NoteShapeType"]   = std::string(magic_enum::enum_name(config.NoteShapeType));
        json["NoteGuideLength"] = config.NoteGuideLength;
        json["MusicVolume"]     = config.MusicVolume;
        json["EffectVolume"]    = config.EffectVolume;

        if (const auto bindings = config.KeyBindings.find(KeyMode::Seven); bindings != config.KeyBindings.end())
        {
            for (const auto& [channel, key] : bindings->second)
                json["KeyBindings"][std::string(magic_enum::enum_name(channel))] = std::string(magic_enum::enum_name(key));
        }

        const auto path = GetConfigPath();

        std::error_code ec;
        std::filesystem::create_directories(path.parent_path(), ec);

        std::ofstream file(path);
        if (!file)
            return;

        file << json.dump(4);
    }
}
