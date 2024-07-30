#ifndef O2JAM_LIST_LOADER_HPP
#define O2JAM_LIST_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>

#include <Genode/UI/List.hpp>
#include <OTwo/Metadata/UI/ListMetadata.hpp>

class ListLoader : public ResourceLoader<Gx::List>
{
public:
    ListLoader() = default;

    Gx::ResourcePtr<Gx::List> LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const override;
    Gx::ResourcePtr<Gx::List> LoadFromMetadata(const ResourceMetadata &metadata, const Gx::ResourceContext &context) const override;

    static bool ParseMetadata(Gx::Json attributes, ListMetadata &metadata, const Gx::ResourceContext &ctx);
};

#endif