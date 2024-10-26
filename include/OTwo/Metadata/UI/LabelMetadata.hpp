#pragma once

#include <OTwo/Metadata/Graphics/TransformMetadata.hpp>
#include <Genode/UI/Label.hpp>

struct LabelMetadata : public TransformMetadata
{
    std::string          String;
    unsigned int         FontSize;
    unsigned int         FontWidth;
    bool                 Bold;
    bool                 Italic;
    bool                 Underlined;
    float                OutlineThickness;
    float                Kerning;
    sf::Color            Color;
    sf::Color            OutlineColor;
    Gx::Label::Alignment Alignment;
};
