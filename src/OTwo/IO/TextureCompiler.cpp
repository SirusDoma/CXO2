#include <OTwo/IO/TextureCompiler.hpp>
#include <Genode/System/Exception.hpp>

#include <algorithm>
#include <stdexcept>

TextureCompiler::TextureCompiler(const unsigned int maxDimension, const unsigned int padding)
    : m_maxDimension(maxDimension), m_padding(padding)
{
}

void TextureCompiler::Add(const sf::Image& image)
{
    if (image.getSize().x == 0 || image.getSize().y == 0)
    {
        throw std::invalid_argument("Cannot add empty image to texture compiler");
    }

    if (image.getSize().x > m_maxDimension || image.getSize().y > m_maxDimension)
    {
        throw std::invalid_argument("Image size exceeds maximum dimension");
    }

    m_textures.emplace_back(TextureInfo(image));
}

SpriteSheet TextureCompiler::Compile()
{
    if (m_textures.empty())
    {
        throw Gx::InvalidOperationException("No textures to compile");
    }

    sf::Vector2u sheetSize;
    if (!TryPackTextures(sheetSize))
    {
        throw Gx::InvalidOperationException("Failed to pack textures within maximum dimension");
    }

    auto sheet   = CreateSheet(sheetSize);
    // auto texture = sf::Texture(sheet);

    std::vector<sf::IntRect> rects;
    rects.reserve(m_textures.size());

    for (const auto& texInfo : m_textures)
    {
        // Return rect without padding
        rects.emplace_back(
            sf::Vector2i(static_cast<int>(texInfo.position.x), static_cast<int>(texInfo.position.y)),
            sf::Vector2i(static_cast<int>(texInfo.size.x), static_cast<int>(texInfo.size.y))
        );
    }

    return SpriteSheet(std::move(sheet), std::move(rects));
}

void TextureCompiler::Clear()
{
    m_textures.clear();
}

size_t TextureCompiler::GetTextureCount() const
{
    return m_textures.size();
}

bool TextureCompiler::TryPackTextures(sf::Vector2u& sheetSize)
{
    if (m_textures.empty())
    {
        return false;
    }

    // Start with border padding
    unsigned int currentX = m_padding;
    unsigned int currentY = m_padding;
    unsigned int rowHeight = 0;
    unsigned int maxWidth = 0;
    unsigned int maxHeight = 0;

    for (auto& texInfo : m_textures)
    {
        // Check if we need to move to next row (accounting for right border padding)
        if (currentX + texInfo.size.x + m_padding > m_maxDimension)
        {
            // Move to next row
            currentX = m_padding;
            currentY += rowHeight + m_padding;
            rowHeight = 0;

            // Check if we exceed vertical dimension (accounting for bottom border padding)
            if (currentY + texInfo.size.y + m_padding > m_maxDimension)
            {
                return false;
            }
        }

        // Place texture at current position
        texInfo.position.x = currentX;
        texInfo.position.y = currentY;

        // Update tracking variables
        currentX += texInfo.size.x + m_padding;
        rowHeight = std::max(rowHeight, texInfo.size.y);
        maxWidth = std::max(maxWidth, currentX);
        maxHeight = std::max(maxHeight, currentY + texInfo.size.y);
    }

    // Add final border padding to sheet size
    sheetSize.x = maxWidth + m_padding;
    sheetSize.y = maxHeight + m_padding;

    return true;
}

sf::Image TextureCompiler::CreateSheet(const sf::Vector2u& sheetSize) const
{
    sf::Image sheet;
    sheet.resize(sheetSize, sf::Color::Transparent);

    for (const auto& texInfo : m_textures)
    {
        if (!sheet.copy(texInfo.image, texInfo.position))
            throw Gx::InvalidOperationException("Failed to write texture to sheet");
    }

    return sheet;
}
