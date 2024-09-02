#include <OTwo/IO/Loaders/SceneGraph/ObjectContainer.hpp>
#include <OTwo/States/State.hpp>

template<typename R>
void ObjectContainer::Add(const std::string &name, Gx::ResourcePtr<R> object, Gx::ResourceContext &ctx)
{
    if (!m_container)
        return;

    if (auto state = dynamic_cast<State*>(m_container); state)
    {
        auto result = state->Import<R>(name, std::move(object), ResourceScope::Local);
        if constexpr (std::is_base_of_v<Gx::Node, R> && !std::is_base_of_v<Gx::Dialog, R>)
        {
            if (!m_importOnly)
                state->AddChild(result);
        }

        return;
    }

    auto &resource = ctx.Store<R>(name, std::move(object));
    if constexpr (std::is_base_of_v<Gx::Node, R> && !std::is_base_of_v<Gx::Dialog, R>)
    {
        if (auto child = dynamic_cast<R*>(&resource); child)
            m_container->AddChild(child);
    }
}
