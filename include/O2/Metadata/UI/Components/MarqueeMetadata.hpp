#ifndef O2JAM_MARQUEE_METADATA_HPP
#define O2JAM_MARQUEE_METADATA_HPP

#include <O2/Metadata/UI/LabelMetadata.hpp>

#include <SFML/Graphics/Rect.hpp>

class MarqueeMetadata : public LabelMetadata
{
public:
    double GetSpeed() const { return m_speed; }
    void SetSpeed(double speed) { m_speed = speed; }

    const sf::FloatRect &GetBounds() const { return m_bounds; }
    void SetBounds(const sf::FloatRect& bounds) { m_bounds = bounds; }

private:
    double m_speed;
    sf::FloatRect m_bounds;
};

#endif
