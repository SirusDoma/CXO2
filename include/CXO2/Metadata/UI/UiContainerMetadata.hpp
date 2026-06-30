#pragma once

#include <CXO2/Metadata/Graphics/TransformMetadata.hpp>

#include <unordered_map>

namespace Cx
{
    struct UiContainerMetadata : public TransformMetadata
    {
        sf::IntRect Bounds = {};
    };
}
