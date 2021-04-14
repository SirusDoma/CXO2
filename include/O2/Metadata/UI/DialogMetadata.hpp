#ifndef O2JAM_DIALOG_METADATA_HPP
#define O2JAM_DIALOG_METADATA_HPP

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/Json.hpp>

#include <O2/Metadata/SpriteMetadata.hpp>
#include <O2/Metadata/UI/ButtonMetadata.hpp>
#include <O2/Metadata/UI/LabelMetadata.hpp>

struct DialogMetadata : public SpriteMetadata
{
    ButtonMetadata AcceptButtonMetadata, CancelButtonMetadata;
    LabelMetadata  PromptLabelMetadata;
};

#endif