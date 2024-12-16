#pragma once

#include <Genode/IO/ResourceLoader.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace Gx
{
    class TextureLoader final : public ResourceLoader<sf::Texture>
    {
    public:
        TextureLoader() = default;
        void UseSmooth(bool smooth);

        [[nodiscard]] ResourcePtr<sf::Texture> LoadFromFile(const std::string& fileName, const ResourceContext& ctx) const override;
        ResourcePtr<sf::Texture> LoadFromMemory(void* data, std::size_t size, const ResourceContext& ctx) const override;
        ResourcePtr<sf::Texture> LoadFromStream(sf::InputStream& stream, const ResourceContext& ctx) const override;

    private:
        bool m_smooth = true;
    };
}
