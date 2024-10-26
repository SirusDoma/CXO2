#pragma once

#include <OTwo/Metadata/Graphics/TransformMetadata.hpp>

struct ListMetadata : public TransformMetadata
{
    int   VerticalCount;
    int   HorizontalCount;
    float VerticalSpacing;
    float HorizontalSpacing;
    Gx::List::Order Order;
    std::vector<TransformMetadata> Layouts = {};

    ResourceType ItemType = ResourceType::None;
    std::string ItemName = std::string();
    Gx::Json ItemSource;
    unsigned int ItemCount = 0;
};
