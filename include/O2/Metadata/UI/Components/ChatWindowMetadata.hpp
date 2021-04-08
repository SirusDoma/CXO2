#ifndef O2JAM_CHAT_WINDOW_METADATA_HPP
#define O2JAM_CHAT_WINDOW_METADATA_HPP

#include <O2/Metadata/TransformMetadata.hpp>

#include <SFML/Graphics/Rect.hpp>

struct ChatWindowMetadata : public TransformMetadata
{
public:
    const sf::FloatRect &GetBounds() const { return m_bounds; }
    void SetBounds(const sf::FloatRect& bounds) { m_bounds = bounds; }

    unsigned int GetFontSize() const { return m_fontSize; }
    void SetFontSize(unsigned int fontSize) { m_fontSize = fontSize; }

    unsigned int GetMaximumChatLength() const { return m_maxChatsLength; }
    void SetMaximumChatLength(unsigned int max) { m_maxChatsLength = max; }

private:
    sf::FloatRect m_bounds;
    unsigned int m_fontSize, m_maxChatsLength;
};

#endif
