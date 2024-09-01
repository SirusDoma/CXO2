#ifndef O2JAM_CURSOR_METADATA_HPP
#define O2JAM_CURSOR_METADATA_HPP

#include <OTwo/Metadata/ResourceMetadata.hpp>
#include <Genode/UI/Cursor.hpp>

struct CursorMetadata : public ResourceMetadata
{
    struct CursorStateMetadata
    {
        Gx::Cursor::Type Type;
        sf::IntRect      TexCoords;
        sf::Vector2u     Hotspot;
    };

    CursorMetadata() : States() {}

    std::vector<CursorStateMetadata> States;
};

#endif