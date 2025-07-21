#pragma once

#include <OTwo/IO/ResourceLoader.hpp>

#include <Genode/UI/List.hpp>
#include <OTwo/Metadata/UI/ListMetadata.hpp>

class ListLoader : public ResourceLoader<Gx::List>
{
public:
    ListLoader() = default;

    Gx::ResourcePtr<Gx::List> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
    Gx::ResourcePtr<Gx::List> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
};
