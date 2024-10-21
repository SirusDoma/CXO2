#ifndef O2JAM_UI_CONTAINER_METADATA_HPP
#define O2JAM_UI_CONTAINER_METADATA_HPP

#include <OTwo/Metadata/Graphics/TransformMetadata.hpp>

#include <unordered_map>

struct UiContainerMetadata : public TransformMetadata
{
    sf::FloatRect Bounds = {};
};

#endif
