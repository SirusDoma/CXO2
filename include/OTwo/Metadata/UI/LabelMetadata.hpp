#pragma once

#include <OTwo/Metadata/Graphics/TransformMetadata.hpp>
#include <Genode/UI/Label.hpp>

struct LabelMetadata : public TransformMetadata
{
    std::string          String;
    unsigned int         FontSize         = 30;
    unsigned int         FontWidth        = 0;
    bool                 Bold             = false;
    bool                 Italic           = false;
    bool                 Underlined       = false;
    float                OutlineThickness = 0.f;
    float                Kerning          = 0.f;
    sf::Color            Color            = sf::Color::White;
    sf::Color            OutlineColor     = sf::Color::Transparent;
    Gx::Label::Alignment Alignment        = Gx::Label::Alignment::None;
};
