#pragma once

#include <OTwo/Metadata/ResourceMetadata.hpp>

#include <Genode/IO/Json.hpp>
#include <Genode/IO/ResourceContext.hpp>
#include <Genode/IO/ResourceLoaderFactory.hpp>

class ObjectContainer;
struct ResourceMetadata;

template<typename R>
class ResourceLoader : public Gx::ResourceLoader<R>
{
public:
    ResourceLoader() = default;

    bool IsStreaming() const override { return Gx::ResourceLoader<R>::IsStreaming(); }

    virtual Gx::ResourcePtr<R> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const = 0;
    virtual Gx::ResourcePtr<R> LoadFromMetadata(const ResourceMetadata& metadata, const Gx::ResourceContext& context) const = 0;

    Gx::ResourcePtr<R> LoadFromFile(const std::string& fileName, const Gx::ResourceContext& ctx) const override;
    Gx::ResourcePtr<R> LoadFromMemory(void* data, std::size_t size, const Gx::ResourceContext& ctx) const override;
    Gx::ResourcePtr<R> LoadFromStream(sf::InputStream& stream, const Gx::ResourceContext& ctx) const override;

protected:
    static void LoadChildren(ObjectContainer& container, const ResourceMetadata& metadata, const Gx::ResourceContext& context);
};

#include <OTwo/IO/ResourceLoader.inl>
