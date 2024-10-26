#pragma once

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/Metadata/UI/LabelMetadata.hpp>

class LabelLoader : public ResourceLoader<Gx::Label>
{
public :
    LabelLoader() = default;

    Gx::ResourcePtr<Gx::Label> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
    Gx::ResourcePtr<Gx::Label> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;

    static bool ParseMetadata(Gx::Json attributes, LabelMetadata& metadata, const Gx::ResourceContext& context);
};
