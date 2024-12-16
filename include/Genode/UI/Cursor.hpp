#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Window/Cursor.hpp>
#include <SFML/Graphics/View.hpp>

#include <unordered_map>

namespace Gx
{
    class Cursor
    {
    public:
        enum class Type
        {
            Arrow,                  //!< Arrow cursor (default)
            ArrowWait,              //!< Busy arrow cursor
            Click,                  //!< Click cursor
            Wait,                   //!< Busy cursor
            Text,                   //!< I-beam, cursor when hovering over a field allowing text entry
            Hand,                   //!< Pointing hand cursor
            SizeHorizontal,         //!< Horizontal double arrow cursor
            SizeVertical,           //!< Vertical double arrow cursor
            SizeTopLeftBottomRight, //!< Double arrow cursor going from top-left to bottom-right
            SizeBottomLeftTopRight, //!< Double arrow cursor going from bottom-left to top-right
            SizeLeft,               //!< Left arrow cursor on Linux, same as SizeHorizontal on other platforms
            SizeRight,              //!< Right arrow cursor on Linux, same as SizeHorizontal on other platforms
            SizeTop,                //!< Up arrow cursor on Linux, same as SizeVertical on other platforms
            SizeBottom,             //!< Down arrow cursor on Linux, same as SizeVertical on other platforms
            SizeTopLeft,            //!< Top-left arrow cursor on Linux, same as SizeTopLeftBottomRight on other platforms
            SizeBottomRight,        //!< Bottom-right arrow cursor on Linux, same as SizeTopLeftBottomRight on other platforms
            SizeBottomLeft,         //!< Bottom-left arrow cursor on Linux, same as SizeBottomLeftTopRight on other platforms
            SizeTopRight,           //!< Top-right arrow cursor on Linux, same as SizeBottomLeftTopRight on other platforms
            SizeAll,                //!< Combination of SizeHorizontal and SizeVertical
            Cross,                  //!< Crosshair cursor
            Help,                   //!< Help cursor
            NotAllowed              //!< Action not allowed cursor
        };

        Cursor();
        explicit Cursor(const sf::Texture& texture, sf::Vector2u hotspot = sf::Vector2u());
        explicit Cursor(const sf::Image& image, sf::Vector2u hotspot = sf::Vector2u());

        bool IsEnabled() const;

        void SetEnabled(bool enabled);

        void Register(Type type, const sf::Texture& texture, sf::Vector2u hotspot = sf::Vector2u());
        void Register(Type type, const sf::Image& image, sf::Vector2u hotspot = sf::Vector2u());

        const sf::Cursor& GetHandle(Type type = Type::Arrow) const;
        Type GetLastRetrievedHandleType() const;

        bool Scale(float scale);

    private:
        struct CursorHandle
        {
            sf::Cursor   Handle = sf::Cursor(sf::Cursor::Type::Arrow);
            sf::Image    Source;
            sf::Vector2u InitialSize;
            sf::Vector2u Hotspot;
        };

        mutable Type m_lastHandleType;
        bool m_enabled;
        float m_scale;

        std::unordered_map<Type, CursorHandle> m_cursors;
    };
}
