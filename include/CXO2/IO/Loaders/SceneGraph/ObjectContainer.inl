#pragma once

#include <CXO2/States/State.hpp>

namespace Cx
{
    template<typename R>
    void ObjectContainer::Add(const std::string& name, Gx::ResourcePtr<R> object, Gx::ResourceContext& ctx)
    {
        if (!m_container)
            return;

        if (const auto state = dynamic_cast<State*>(m_container); state)
        {
            auto result = state->Import<R>(name, std::move(object), ResourceScope::Local);
            if constexpr (std::is_base_of_v<Gx::Node, R>)
            {
                result->SetName(name);
                if (!m_importOnly && result && !std::is_base_of_v<Gx::Dialog, R>)
                    state->AddChild(*result);
            }
        }
        else
        {
            auto& resource = ctx.Store<R>(name, std::move(object));
            if constexpr (std::is_base_of_v<Gx::Node, R>)
            {
                resource.SetName(name);
                if constexpr (!std::is_base_of_v<Gx::Dialog, R>)
                    m_container->AddChild(resource);
            }
        }
    }
}
