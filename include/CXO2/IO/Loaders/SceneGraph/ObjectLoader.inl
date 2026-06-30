#pragma once

#include <CXO2/Decorators/IO/ResourceContextDecorator.hpp>

namespace Cx
{
    template<typename R>
    Gx::ResourcePtr<R> ObjectLoader::LoadFromJson(const std::string& name, const Gx::Json& json, const Gx::ResourceContext& context)
    {
        auto loader = Gx::ResourceLoaderFactory::CreateLoader<R>();
        if (!loader)
            return nullptr;

        Gx::ResourcePtr<R> resource;
        auto ctx = Gx::ResourceContext::Rebind(context, name);
        if (json.type() == Gx::Json::value_t::string)
            resource = loader->LoadFromFile(json.get<std::string>(), ctx);
        else
            resource = loader->LoadFromJson(json, ctx);

        if (!resource)
            return nullptr;

        if constexpr (std::is_base_of_v<Gx::Node, R>)
            resource->SetName(name);

        return resource;
    }

    template<typename R>
    Gx::ResourcePtr<R> ObjectLoader::LoadFromJson(const std::string& name, const std::string& type, const Gx::Json& json, const Gx::ResourceContext& context)
    {
        auto loader = Gx::ResourceLoaderFactory::CreateLoader<R>(type);
        if (!loader)
            return nullptr;

        Gx::ResourcePtr<R> resource;
        auto ctx = ResourceContextDecorator::Rebind(context, name);
        if (json.type() == Gx::Json::value_t::string)
            resource = loader->LoadFromFile(json.get<std::string>(), ctx);
        else
            resource = loader->LoadFromJson(json, ctx);

        if (!resource)
            return nullptr;

        if constexpr (std::is_base_of_v<Gx::Node, R>)
            resource->SetName(name);

        return resource;
    }
}
