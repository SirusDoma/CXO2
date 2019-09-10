#ifndef SPRITE_DEFINITION_HPP
#define SPRITE_DEFINITION_HPP

#include <Genode/IO/ResourceDefinition.hpp>
#include <Genode/IO/Json.hpp>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

class SpriteDefinition : public Gx::ResourceDefinition
{
public:
    SpriteDefinition() {}
    virtual ~SpriteDefinition() {}

    sf::Color   Color;
    sf::IntRect TexCoords;
};

#endif