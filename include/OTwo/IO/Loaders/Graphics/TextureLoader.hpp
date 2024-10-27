#pragma once

#include <Genode/IO/ResourceLoader.hpp>
#include <SFML/Graphics/Texture.hpp>

class TextureLoader final : public Gx::ResourceLoader<sf::Texture>
{
public:
    TextureLoader() = default;
    void UseSmooth(bool smooth);

    Gx::ResourcePtr<sf::Texture> LoadFromFile(const std::string& fileName, const Gx::ResourceContext& ctx) const override;
    Gx::ResourcePtr<sf::Texture> LoadFromMemory(void* data, std::size_t size, const Gx::ResourceContext& ctx) const override;
    Gx::ResourcePtr<sf::Texture> LoadFromStream(sf::InputStream& stream, const Gx::ResourceContext& ctx) const override;

private:
    bool m_smooth = true;
};
