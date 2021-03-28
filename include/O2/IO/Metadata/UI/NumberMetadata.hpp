#ifndef NUMBER_METADATA_HPP
#define NUMBER_METADATA_HPP

#include <SFML/Graphics/Rect.hpp>
#include <O2/IO/Metadata/TransformMetadata.hpp>

#include <unordered_map>

class NumberMetadata : public TransformMetadata
{
public:
    NumberMetadata() : m_digitFrames() {}

    const sf::Color &GetColor() const { return m_color; }
    void SetColor(const sf::Color &color) { m_color = color; }

    const sf::Vector2u &GetDigitSize() const { return m_digitSize; }
    void SetDigitSize(const sf::Vector2u &digitSize) { m_digitSize = digitSize; }

    float GetLetterSpacing() const { return m_letterSpacing; }
    void SetLetterSpacing(float letterSpacing) { m_letterSpacing = letterSpacing; }

    const std::unordered_map<unsigned int, sf::IntRect> &GetDigitFrames() const { return m_digitFrames; }
    void SetDigitFrame(unsigned int number, const sf::IntRect &frame) { m_digitFrames[number] = frame; }
    
private:
    using DigitFrames = std::unordered_map<unsigned int, sf::IntRect>;

    sf::Color    m_color;
    sf::Vector2u m_digitSize;
    float        m_letterSpacing;
    DigitFrames  m_digitFrames;
};

#endif
