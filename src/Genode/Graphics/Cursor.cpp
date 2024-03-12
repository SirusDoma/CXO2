#include <Genode/Graphics/Cursor.hpp>

namespace Gx
{
    Cursor::Cursor() :
        m_cursor()
    {
    }

    Cursor::~Cursor()
    {
    }

    Cursor::Cursor(const sf::Texture& texture) :
        Cursor::Cursor()
    {
        auto image = texture.copyToImage();
        m_size     = image.getSize();
        m_source   = const_cast<unsigned char*>(image.getPixelsPtr());

        m_cursor.loadFromPixels(&m_source[0], m_size, sf::Vector2u(0, 0));
    }

    sf::Cursor &Cursor::GetHandle() const
    {
        return m_cursor;
    }

}
