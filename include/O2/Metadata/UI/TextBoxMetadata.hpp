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

private:
    sf::FloatRect m_bounds;
    unsigned int m_maxLength;
};

#endif
