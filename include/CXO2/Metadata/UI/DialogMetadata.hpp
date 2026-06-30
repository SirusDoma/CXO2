#pragma once

#include <CXO2/Metadata/ResourceMetadata.hpp>
#include <Genode/IO/Json.hpp>

#include <CXO2/Metadata/Graphics/SpriteMetadata.hpp>
#include <CXO2/Metadata/UI/ButtonMetadata.hpp>
#include <CXO2/Metadata/UI/LabelMetadata.hpp>

namespace Cx
{
    struct DialogMetadata : public SpriteMetadata
    {
        ButtonMetadata AcceptButtonMetadata, CancelButtonMetadata;
        LabelMetadata  PromptLabelMetadata;
    };
}
