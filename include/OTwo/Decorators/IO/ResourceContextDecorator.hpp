#ifndef O2JAM_RESOURCE_CONTEXT_HPP
#define O2JAM_RESOURCE_CONTEXT_HPP

#include <OTwo/Metadata/ResourceMetadata.hpp>

#include <Genode/IO/Json.hpp>
#include <Genode/IO/Resource.hpp>
#include <Genode/IO/ResourceContext.hpp>

// TODO: Consider using ContextBuilder from ResourceManager

class ResourceContextDecorator : public Gx::ResourceContext
{
public:
    static ResourceContextDecorator Decorate(const Gx::ResourceContext &ctx);

    template<typename R>
    R* Instantiate(const ResourceMetadata &metadata, const std::string& newID) const;

    template<typename R>
    R* Find() const;

    template<typename R>
    R* Find(const ResourceMetadata &metadata, Gx::CacheMode cacheMode = Gx::CacheMode::Reuse) const;

private:
    explicit ResourceContextDecorator(const Gx::ResourceContext &ctx);

    template<typename R>
    Gx::ResourcePtr<R> Deserialize(const std::string &id) const;
};

#include <OTwo/Decorators/IO/ResourceContextDecorator.inl>
#endif
