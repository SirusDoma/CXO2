#ifndef O2JAM_TEXTBOX_METADATA_HPP
#define O2JAM_TEXTBOX_METADATA_HPP

#include <O2/Metadata/UI/LabelMetadata.hpp>
#include <SFML/Graphics/Rect.hpp>

class TextBoxMetadata : public LabelMetadata
{
public:
    const sf::FloatRect &GetBounds() const { return m_bounds; }
    void SetBounds(const sf::FloatRect& bounds) { m_bounds = bounds; }

    unsigned int GetMaximumLength() const { return m_maxLength; }
    void SetMaximumLength(unsigned int maxLength) { m_maxLength = maxLength; }

    const sf::Color &GetHighlightTextColor() const { return m_highlightTextColor; }
    void SetHighlightTextColor(const sf::Color &color) { m_highlightTextColor = color; }

    const sf::Color &GetHighlightBackColor() const { return m_highlightBackColor; }
    void SetHighlightBackColor(const sf::Color &color) { m_highlightBackColor = color; }

private:
    sf::FloatRect m_bounds;
    unsigned int m_maxLength;
    sf::Color m_highlightTextColor, m_highlightBackColor;
};

#endif
