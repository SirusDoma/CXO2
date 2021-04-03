#ifndef LABEL_METADATA_HPP
#define LABEL_METADATA_HPP

#include <O2/IO/Metadata/TransformMetadata.hpp>

class LabelMetadata : public TransformMetadata
{
public:
    const std::string &GetString() const { return m_string; }
    void SetString(const std::string &string) { m_string = string; }

    unsigned int GetFontSize() const { return m_fontSize; }
    void SetFontSize(unsigned int fontSize) { m_fontSize = fontSize; }

    const sf::Color &GetColor() const { return m_color; }
    void SetColor(const sf::Color& color) { m_color = color; }

    const float GetOutlineThickness() const { return m_outlineThickness; }
    void SetOutlineThickness(float thickness) { m_outlineThickness = thickness; }

    const sf::Color &GetOutlineColor() const { return m_outlineColor; }
    void SetOutlineColor(const sf::Color& color) { m_outlineColor = color; }

private:
    std::string m_string;
    unsigned int m_fontSize;
    float m_outlineThickness;
    sf::Color m_color;
    sf::Color m_outlineColor;
};

#endif
