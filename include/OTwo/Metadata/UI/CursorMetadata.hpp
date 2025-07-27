#pragma once

#include <OTwo/Metadata/ResourceMetadata.hpp>
#include <Genode/UI/Cursor.hpp>

struct CursorMetadata : public ResourceMetadata
{
    struct CursorStateMetadata
    {
        std::optional<std::uint32_t> ID = std::nullopt;
        Gx::Cursor::Type Type;
        sf::IntRect      TexCoords;
        sf::Vector2u     Hotspot;
    };

    CursorMetadata() : States() {}

    std::vector<CursorStateMetadata> States;
};
