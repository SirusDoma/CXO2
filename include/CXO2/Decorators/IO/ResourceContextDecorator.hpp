#pragma once

#include <CXO2/Metadata/ResourceMetadata.hpp>

#include <Genode/IO/Json.hpp>
#include <Genode/IO/Resource.hpp>
#include <Genode/IO/ResourceContext.hpp>
#include <Genode/SceneGraph/Node.hpp>

namespace Cx
{
    class ResourceContextDecorator : public Gx::ResourceContext
    {
    public:
        static ResourceContextDecorator Decorate(const Gx::ResourceContext& ctx);
        static ResourceContextDecorator Decorate(const Gx::ResourceContext& ctx, const ResourceMetadata& parent);
        static ResourceContextDecorator Rebind(const Gx::ResourceContext& ctx, const std::string& name);

        template<typename R>
        R* Require() const;

        template<typename R>
        R* Require(const ResourceMetadata& metadata, Gx::CacheMode cacheMode = Gx::CacheMode::Reuse) const;

        const ResourceMetadata* GetParent() const;
        sf::IntRect GetParentBound() const;

    private:
        explicit ResourceContextDecorator(const Gx::ResourceContext& ctx);
        explicit ResourceContextDecorator(const Gx::ResourceContext& ctx, const ResourceMetadata& parent);

        virtual std::string Format(const std::string& fileName, const std::string& format) const;

        template<typename R>
        Gx::ResourcePtr<R> Deserialize(const std::string& id) const;

        const ResourceMetadata* m_parent;
    };

}

#include <CXO2/Decorators/IO/ResourceContextDecorator.inl>
