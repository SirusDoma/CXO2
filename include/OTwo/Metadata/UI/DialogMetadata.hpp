#ifndef O2JAM_DIALOG_METADATA_HPP
#define O2JAM_DIALOG_METADATA_HPP

#include <OTwo/Metadata/ResourceMetadata.hpp>
#include <Genode/IO/Json.hpp>

#include <OTwo/Metadata/Graphics/SpriteMetadata.hpp>
#include <OTwo/Metadata/UI/ButtonMetadata.hpp>
#include <OTwo/Metadata/UI/LabelMetadata.hpp>

struct DialogMetadata : public SpriteMetadata
{
    ButtonMetadata AcceptButtonMetadata, CancelButtonMetadata;
    LabelMetadata  PromptLabelMetadata;
};

#endif