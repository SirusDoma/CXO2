#ifndef PROGRESS_BAR_METADATA_LOADER_HPP
#define PROGRESS_BAR_METADATA_LOADER_HPP

#include <Genode/IO/Json.hpp>
#include <Genode/IO/MetadataLoader.hpp>

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/ResourceContext.hpp>
#include <Genode/UI/ProgressBar.hpp>

#include <O2/IO/Metadata/UI/ProgressBarMetadata.hpp>

class ProgressBarMetadataLoader : public Gx::MetadataLoader<Gx::ProgressBar>
{
public :
    ProgressBarMetadataLoader();

    virtual Gx::ResourceMetadata* Load(Gx::Uint8* data, Gx::Uint64 size) const;
    virtual Gx::ProgressBar* Create(Gx::ResourceMetadata* definition, Gx::ResourceContext context) const;
};

#endif