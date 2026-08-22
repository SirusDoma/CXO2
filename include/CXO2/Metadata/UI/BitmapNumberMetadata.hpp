#pragma once

#include <CXO2/Metadata/Graphics/TransformMetadata.hpp>
#include <CXO2/UI/BitmapNumber.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <unordered_map>

namespace Cx
{
    struct BitmapNumberMetadata : public TransformMetadata
    {
        sf::Color    Color                    = sf::Color::White;
        sf::Vector2u DigitSize                = {};
        float        Kerning                  = 0.f;
        unsigned int Value                    = 0;
        unsigned int DigitCount               = 1;
        sf::Time     Duration                 = sf::Time::Zero;
        BitmapNumber::Alignment Alignment     = BitmapNumber::Alignment::None;
        Gx::BlendMode BlendMode               = Gx::BlendMode::Auto;
        std::unordered_map<unsigned int, std::vector<sf::IntRect>> DigitFrames = {};
    };
}
