#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <vector>
#include <memory>

namespace Cx
{
    struct SpriteSheet
    {
        sf::Image Source = {};
        sf::Texture Texture = {};
        std::vector<sf::IntRect> TexCoords = {};
        std::vector<sf::IntRect> Frames = {};

        SpriteSheet(sf::Image&& src, std::vector<sf::IntRect>&& coords)
            : Source(std::move(src)), TexCoords(std::move(coords))
        {
        }

        SpriteSheet(sf::Texture&& tex, std::vector<sf::IntRect>&& coords)
            : Texture(std::move(tex)), TexCoords(std::move(coords))
        {
        }

        sf::Texture& GetTexture()
        {
            if (Texture.getSize() == sf::Vector2u())
            {
                Texture = sf::Texture(Source);
                Source  = sf::Image();

                Texture.setSmooth(true);
            }

            return Texture;
        }
    };

    class TextureCompiler
    {
    public:
        explicit TextureCompiler(unsigned int maxDimension = 1024, unsigned int padding = 0);

        void Add(const sf::Image& image);
        SpriteSheet Compile();

        void Clear();
        size_t GetTextureCount() const;

    private:
        struct TextureInfo
        {
            sf::Image image;
            sf::Vector2u size;
            sf::Vector2u position;

            explicit TextureInfo(const sf::Image& img)
                : image(img), size(img.getSize()), position(0, 0) {}
        };

        unsigned int m_maxDimension;
        unsigned int m_padding;
        std::vector<TextureInfo> m_textures;

        bool TryPackTextures(sf::Vector2u& sheetSize);
        sf::Image CreateSheet(const sf::Vector2u& sheetSize) const;
    };
}
