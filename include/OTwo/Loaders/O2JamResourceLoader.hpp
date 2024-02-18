#ifndef O2JAM_RESOURCE_LOADER_HPP
#define O2JAM_RESOURCE_LOADER_HPP

#include <Genode/IO/Json.hpp>
#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/ResourceContext.hpp>
#include <Genode/IO/ResourceLoaderFactory.hpp>

template<typename R>
class O2JamResourceLoader : public Gx::ResourceLoader<R>
{
public:
    O2JamResourceLoader();

    virtual bool IsMetadataRequired() const;
    virtual bool IsStreamed() const;

    virtual std::unique_ptr<Gx::ResourceMetadata> LoadMetadata(const void* data, std::size_t size) const = 0;
    virtual Gx::ResourcePtr<R> Load(const Gx::ResourceMetadata& metadata, const Gx::ResourceContext& context = Gx::ResourceContext()) const = 0;
    virtual Gx::ResourcePtr<R> Load(const void* data, std::size_t size) const;

    static void ParseReferences(Json require, Gx::ResourceMetadata &metadata);
};

#include <OTwo/Loaders/O2JamResourceLoader.inl>
#endif
