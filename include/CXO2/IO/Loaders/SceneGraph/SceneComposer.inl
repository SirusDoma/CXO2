#pragma once

#include <CXO2/Decorators/IO/ResourceContextDecorator.hpp>

#include <Genode/IO/ResourceLoaderFactory.hpp>

namespace Cx
{
    template<typename R>
    void SceneComposer::Add(const std::string& name, const Gx::Json& json, Gx::ResourceContext& context)
    {
        Add<R>(name, std::string(), json, context);
    }

    template<typename R>
    void SceneComposer::Add(const std::string& name, const std::string& type, const Gx::Json& json, Gx::ResourceContext& context)
    {
        const auto loader = type.empty()
            ? Gx::ResourceLoaderFactory::CreateLoader<R>()
            : Gx::ResourceLoaderFactory::CreateLoader<R>(type);

        if (!loader)
            return;

        Gx::ResourcePtr<R> resource;
        auto ctx = ResourceContextDecorator::Rebind(context, name);
        if (json.type() == Gx::Json::value_t::string)
            resource = loader->LoadFromFile(json.get<std::string>(), ctx);
        else
            resource = loader->LoadFromJson(json, ctx);

        if (!resource)
            return;

        if constexpr (std::is_base_of_v<Gx::Node, R>)
            resource->SetName(name);

        Place(name, std::move(resource), ctx);
    }

    template<typename R>
    void SceneComposer::Place(const std::string& name, Gx::ResourcePtr<R> object, Gx::ResourceContext& context)
    {
        if (!object)
            return;

        if (const auto state = m_state; state)
        {
            auto result = state->Import<R>(name, std::move(object), ResourceScope::Local);
            if (!result)
                return;

            if constexpr (std::is_base_of_v<Gx::Node, R>)
            {
                result->SetName(name);
                if (m_instantiate && !dynamic_cast<Cx::Dialog*>(result))
                    state->AddChild(*result);
            }
        }
        else
        {
            auto& resource = context.Store<R>(name, std::move(object));
            if constexpr (std::is_base_of_v<Gx::Node, R>)
            {
                resource.SetName(name);
                if (m_instantiate && !dynamic_cast<Cx::Dialog*>(&resource))
                    m_target->AddChild(resource);
            }
        }
    }
}
