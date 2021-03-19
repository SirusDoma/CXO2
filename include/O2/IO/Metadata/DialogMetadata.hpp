#ifndef DIALOG_METADATA_HPP
#define DIALOG_METADATA_HPP

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/Json.hpp>

#include <O2/IO/Metadata/SpriteMetadata.hpp>
#include <O2/IO/Metadata/ButtonMetadata.hpp>
#include <O2/IO/Metadata/LabelMetadata.hpp>

struct DialogMetadata : public SpriteMetadata
{
public:
    DialogMetadata() {}
    virtual ~DialogMetadata() {}

    ButtonMetadata AcceptButton, CancelButton;
    LabelMetadata PromptLabel;
};

#endif