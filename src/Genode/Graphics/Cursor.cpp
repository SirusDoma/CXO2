#include <Genode/Graphics/Cursor.hpp>

namespace Gx
{
    Cursor::Cursor() :
        m_cursor()
    {
    }

    Cursor::Cursor(const sf::Texture& texture) :
        Cursor::Cursor()
    {
        auto image  = texture.copyToImage();
        auto size   = image.getSize();
        auto source = image.getPixelsPtr();
        int count   = size.x * size.y * 4;

        for (int i = 0; i < count; i++)
        {
            m_pixels.push_back(*source);
            source++;
        }

        m_cursor = new sf::Cursor();
        m_cursor->loadFromPixels(&m_pixels[0], size, sf::Vector2u(0, 0));
    }

    sf::Cursor* Cursor::GetHandle()
    {
        return m_cursor;
    }
}