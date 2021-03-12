#ifndef MUSIC_DEFINITION_HPP
#define MUSIC_DEFINITION_HPP

#include <Genode/IO/ResourceDefinition.hpp>
#include <Genode/IO/Json.hpp>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

class MusicDefinition : public Gx::ResourceDefinition
{
public:
    MusicDefinition() {}
    virtual ~MusicDefinition() {}

    bool Loop;
};

#endif