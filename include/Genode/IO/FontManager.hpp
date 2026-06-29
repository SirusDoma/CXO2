#pragma once

#include <Genode/IO/ResourceContainer.hpp>

#include <SFML/Graphics/Font.hpp>

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <utility>
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

        bool Store(const std::string& key, std::vector<std::uint8_t> bytes);

        void Clear();

    private:
        mutable std::mutex m_mutex;
        ResourceContainer<std::vector<std::uint8_t>> m_cache;
    };
}
