#pragma once

#include <OTwo/States/State.hpp>

template<typename R>
void ObjectContainer::Add(const std::string& name, Gx::ResourcePtr<R> object, Gx::ResourceContext& ctx)
{
    if (!m_container)
        return;

    if (const auto state = dynamic_cast<State*>(m_container); state)
    {
        auto result = state->Import<R>(name, std::move(object), ResourceScope::Local);
        if constexpr (std::is_base_of_v<Gx::Node, R> && !std::is_base_of_v<Gx::Dialog, R>)
        {
            if (!m_importOnly && result)
                state->AddChild(*result);
        }
    }
    else
    {
        auto& resource = ctx.Store<R>(name, std::move(object));
        if constexpr (std::is_base_of_v<Gx::Node, R> && !std::is_base_of_v<Gx::Dialog, R>)
            m_container->AddChild(resource);
    }
}
