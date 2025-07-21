#pragma once

#include <OTwo/Metadata/Graphics/TransformMetadata.hpp>
#include <Genode/UI/BitmapNumber.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <unordered_map>

struct BitmapNumberMetadata : public TransformMetadata
{
    sf::Color    Color                    = sf::Color::White;
    sf::Vector2u DigitSize                = {};
    float        Kerning                  = 0.f;
    unsigned int Value                    = 0;
    unsigned int DigitCount               = 1;
    sf::Time     Duration                 = sf::Time::Zero;
    Gx::BitmapNumber::Alignment Alignment = Gx::BitmapNumber::Alignment::None;
    Gx::BlendMode BlendMode               = Gx::BlendMode::Auto;
    std::unordered_map<unsigned int, std::vector<sf::IntRect>> DigitFrames = {};
};
