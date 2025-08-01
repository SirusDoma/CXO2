#pragma once

#include <SFML/Graphics/Font.hpp>

#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <mutex>

namespace Gx
{
    class FontManager
    {
    public:
        FontManager() = default;

        [[nodiscard]] std::unique_ptr<sf::Font> Create(const std::string& nameOrPath);
        [[nodiscard]] std::unique_ptr<sf::Font> CreateDefault();

        [[nodiscard]] std::optional<std::pair<const void*, std::size_t>> GetData(const std::string& key);
        [[nodiscard]] std::optional<std::pair<const void*, std::size_t>> GetDefaultData();

        [[nodiscard]] std::vector<std::string> GetKeys() const;

        bool Store(const std::string& key, std::vector<std::uint8_t> bytes);

        void Clear();

    private:
        [[nodiscard]] static std::optional<std::string> ResolveFontPath(const std::string& nameOrPath);
        [[nodiscard]] static std::optional<std::string> ResolveDefaultFontPath();

        [[nodiscard]] static std::optional<std::vector<std::uint8_t>> ReadAllBytes(const std::string& filePath);

        [[nodiscard]] static std::string NormalizeKey(std::string_view nameOrPath);

        mutable std::mutex m_mutex;
        std::unordered_map<std::string, std::vector<std::uint8_t>> m_cache;
    };
}


