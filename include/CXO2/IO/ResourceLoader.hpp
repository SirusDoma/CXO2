#pragma once

#include <Genode/IO/Json.hpp>
#include <Genode/IO/ResourceContext.hpp>

#include <filesystem>

namespace Cx
{
    class SceneComposer;
    struct ResourceMetadata;

    template<typename R>
    class ResourceLoader : public Gx::ResourceLoader<R>
    {
    public:
        using Builder = typename Gx::ResourceLoader<R>::Builder;

        ResourceLoader() = default;

        bool IsStreaming() const override { return Gx::ResourceLoader<R>::IsStreaming(); }

        [[nodiscard]] virtual bool IsFailSafe() const { return true; }

        template<typename U>
        static void OnRegistered(const U& id, const Builder& builder);

        template<typename U>
        static void OnRemoved(const U& id);

        [[nodiscard]] virtual Gx::ResourcePtr<R> LoadFromMetadata(const ResourceMetadata& metadata, const Gx::ResourceContext& context) const = 0;

        [[nodiscard]] Gx::ResourcePtr<R> LoadFromFile(const std::filesystem::path& fileName, const Gx::ResourceContext& ctx) const override;
        [[nodiscard]] Gx::ResourcePtr<R> LoadFromMemory(void* data, std::size_t size, const Gx::ResourceContext& ctx) const override;
        [[nodiscard]] Gx::ResourcePtr<R> LoadFromStream(sf::InputStream& stream, const Gx::ResourceContext& ctx) const override;
        [[nodiscard]] Gx::ResourcePtr<R> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& ctx) const override = 0;

    protected:
        static void LoadChildren(SceneComposer& composer, const ResourceMetadata& metadata, const Gx::ResourceContext& context);
    };

}

#include <CXO2/IO/ResourceLoader.inl>
