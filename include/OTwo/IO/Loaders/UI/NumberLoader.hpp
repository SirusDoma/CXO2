#ifndef O2JAM_NUMBER_LOADER_HPP
#define O2JAM_NUMBER_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>
#include <Genode/UI/Number.hpp>

class NumberLoader : public ResourceLoader<Gx::Number>
{
public :
    NumberLoader() = default;

    Gx::ResourcePtr<Gx::Number> LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const override;
    Gx::ResourcePtr<Gx::Number> LoadFromMetadata(const ResourceMetadata &metadata, const Gx::ResourceContext &context) const override;
};


#endif