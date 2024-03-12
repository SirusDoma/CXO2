#ifndef GENODE_IO_FONT_LOADER_HPP
#define GENODE_IO_FONT_LOADER_HPP

#include <Genode/IO/ResourceLoader.hpp>

#include <SFML/Graphics/Font.hpp>

namespace Gx
{
    class FontLoader final : public ResourceLoader<sf::Font>
    {
    private:
        bool m_smooth = true;

    public:
        FontLoader() = default;

        bool IsStreaming() const override;
        void UseSmooth(bool smooth);

        ResourcePtr<sf::Font> LoadFromFile(const std::string &fileName, const ResourceContext &ctx) const override;
        ResourcePtr<sf::Font> LoadFromMemory(void *data, std::size_t size, const ResourceContext &ctx) const override;
        ResourcePtr<sf::Font> LoadFromStream(sf::InputStream &stream, const ResourceContext &ctx) const override;
    };
}

#endif