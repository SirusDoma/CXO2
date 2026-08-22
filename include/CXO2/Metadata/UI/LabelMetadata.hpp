#pragma once

#include <CXO2/Metadata/Graphics/TransformMetadata.hpp>
#include <CXO2/UI/Label.hpp>

namespace Cx
{
    struct LabelMetadata : public TransformMetadata
    {
        std::string              String;
        std::string              Ellipsis;
        bool                     AllowOverflow     = false;
        unsigned int             FontSize          = 30;
        unsigned int             FontWidth         = 0;
        bool                     Bold              = false;
        bool                     Italic            = false;
        bool                     Underlined        = false;
        float                    OutlineThickness  = 0.f;
        float                    Kerning           = 0.f;
        sf::Color                Color             = sf::Color::White;
        sf::Color                OutlineColor      = sf::Color::Transparent;
        sf::FloatRect            Bounds            = sf::FloatRect();
        Gx::Text::LineAlignment  Alignment         = Gx::Text::LineAlignment::Default;
        Label::VerticalAlignment VerticalAlignment = Label::VerticalAlignment::Center;
    };
}
