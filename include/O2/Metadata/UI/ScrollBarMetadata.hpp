#ifndef O2JAM_SCROLL_BAR_METADATA_HPP
#define O2JAM_SCROLL_BAR_METADATA_HPP

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <Genode/UI/ScrollBar.hpp>
#include <O2/Metadata/SpriteMetadata.hpp>

struct ScrollBarMetadata : public SpriteMetadata
{
public:
    const sf::FloatRect &GetBounds() const { return m_bounds; }
    void SetBounds(const sf::FloatRect& bounds) { m_bounds = bounds; }

    float GetStep() const { return m_step; }
    void SetStep(float step) { m_step = step; }

    float GetMaximum() const { return m_maximum; }
    void SetMaximum(float maximum) { m_maximum = maximum; }

    Gx::ScrollBar::ScrollOrientation GetOrientation() const { return m_orientation; }
    void SetOrientation(Gx::ScrollBar::ScrollOrientation orientation) { m_orientation = orientation; }

private:
    float m_step, m_maximum;
    sf::FloatRect m_bounds;
    Gx::ScrollBar::ScrollOrientation m_orientation;
};

#endif