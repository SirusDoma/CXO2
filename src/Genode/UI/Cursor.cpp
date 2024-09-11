#include <Genode/UI/Cursor.hpp>
#include <Genode/IO/IOException.hpp>

#include <SFML/Graphics/Image.hpp>

#include <magic_enum.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "../../../build/cmake-build-release/_deps/sfml-src/include/SFML/Graphics/RenderTexture.hpp"

namespace Gx
{
    Cursor::Cursor() :
        m_lastHandleType(Type::Arrow),
        m_enabled(true),
        m_scale(1.0f),
        m_cursors()
    {
        m_cursors[Type::Arrow] = CursorHandle{};
    }

    Cursor::Cursor(const sf::Texture& texture, const sf::Vector2u hotspot) :
        Cursor(texture.copyToImage(), hotspot)
    {
    }

    Cursor::Cursor(const sf::Image& image, const sf::Vector2u hotspot) :
        Cursor()
    {
        Register(Type::Arrow, image, hotspot);
    }

    bool Cursor::IsEnabled() const
    {
        return m_enabled;
    }

    void Cursor::SetEnabled(const bool enabled)
    {
        m_enabled = enabled;
    }

    void Cursor::Register(const Type type, const sf::Texture &texture, const sf::Vector2u hotspot)
    {
        Register(type, texture.copyToImage(), hotspot);
    }

    auto Cursor::Register(const Type type, const sf::Image &image, const sf::Vector2u hotspot) -> void
    {
        const auto size  = image.getSize();
        const auto data  = const_cast<unsigned char*>(image.getPixelsPtr());

        auto handle = CursorHandle{};
        if (!handle.Handle.createFromPixels(&data[0], size, hotspot))
            throw ResourceLoadException(std::string(magic_enum::enum_name(type)), "Failed to load cursor");

        handle.Source      = image;
        handle.Hotspot     = hotspot;
        handle.InitialSize = image.getSize();
        m_cursors[type]    = std::move(handle);
    }

    const sf::Cursor& Cursor::GetHandle(const Type type) const
    {
        m_lastHandleType = type;

        static auto defaultCursor = sf::Cursor(sf::Cursor::Type::Arrow);
        if (!m_enabled)
            return defaultCursor;

        if (const auto it = m_cursors.find(type); it != m_cursors.end())
            return it->second.Handle;

        if (const auto it = m_cursors.find(Type::Arrow); it != m_cursors.end())
            return it->second.Handle;

        return defaultCursor;
    }

    Cursor::Type Cursor::GetLastRetrievedHandleType() const
    {
        return m_lastHandleType;
    }

    bool Cursor::Scale(const float scale)
    {
        if (m_scale == scale)
            return false;

        m_scale = scale;
        auto target = sf::RenderTexture();
        for (auto& [type, cursor] : m_cursors)
        {
            const auto targetSize = sf::Vector2u(cursor.InitialSize.x * scale, cursor.InitialSize.y * scale);
            if (!target.resize(targetSize))
                throw ResourceLoadException(std::string(magic_enum::enum_name(type)), "Failed to create render texture");

            auto texture = sf::Texture();
            if (!texture.loadFromImage(cursor.Source))
                throw ResourceLoadException(std::string(magic_enum::enum_name(type)), "Failed to load cursor texture");

            target.clear(sf::Color::Transparent);
            {
                auto sprite = sf::Sprite(texture);
                sprite.setScale({scale, scale});
                target.draw(sprite);
            }
            target.display();

            auto result     = target.getTexture().copyToImage();
            const auto size = result.getSize();
            const auto data = const_cast<unsigned char*>(result.getPixelsPtr());

            if (!cursor.Handle.createFromPixels(&data[0], size, sf::Vector2u(cursor.Hotspot.x * scale, cursor.Hotspot.y * scale)))
                throw ResourceLoadException(std::string(magic_enum::enum_name(type)), "Failed to load cursor");
        }

        return true;
    }
}
