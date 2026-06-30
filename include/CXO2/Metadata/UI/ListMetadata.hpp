#pragma once

#include <CXO2/Metadata/Graphics/TransformMetadata.hpp>

namespace Cx
{
    struct ListMetadata : public TransformMetadata
    {
        int   VerticalCount     = 1;
        int   HorizontalCount   = 1;
        float VerticalSpacing   = 0.f;
        float HorizontalSpacing = 0.f;
        Gx::List::Order Order   = Gx::List::Order::Vertical;
        std::vector<TransformMetadata> Layouts = {};

        ResourceType ItemType  = ResourceType::None;
        std::string ItemName   = std::string();
        Gx::Json ItemSource    = Gx::Json();
        unsigned int ItemCount = 0;
    };
}
