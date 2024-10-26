#pragma once

#include <OTwo/Metadata/Graphics/TransformMetadata.hpp>

#include <unordered_map>

struct UiContainerMetadata : public TransformMetadata
{
    sf::FloatRect Bounds = {};
};
