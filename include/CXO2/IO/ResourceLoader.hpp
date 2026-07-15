#pragma once

#include <Genode/IO/Json.hpp>
#include <Genode/IO/ResourceContext.hpp>

#include <filesystem>

namespace Cx
{
    class ObjectContainer;
    struct ResourceMetadata;

    template<typename R>
    class ResourceLoader : public Gx::ResourceLoader<R>
    {
    public:
        ResourceLoader() = default;

        bool IsStreaming() const override { return Gx::ResourceLoader<R>::IsStreaming(); }

        template<typename U>
        static void OnRegistered(const U& id);

        template<typename U>
        static void OnRemoved(const U& id);

        virtual Gx::ResourcePtr<R> LoadFromMetadata(const ResourceMetadata& metadata, const Gx::ResourceContext& context) const = 0;

        Gx::ResourcePtr<R> LoadFromFile(const std::filesystem::path& fileName, const Gx::ResourceContext& ctx) const override;
        Gx::ResourcePtr<R> LoadFromMemory(void* data, std::size_t size, const Gx::ResourceContext& ctx) const override;
        Gx::ResourcePtr<R> LoadFromStream(sf::InputStream& stream, const Gx::ResourceContext& ctx) const override;
        Gx::ResourcePtr<R> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& ctx) const override = 0;

    protected:
        static void LoadChildren(ObjectContainer& container, const ResourceMetadata& metadata, const Gx::ResourceContext& context);
    };

}

#include <CXO2/IO/ResourceLoader.inl>
