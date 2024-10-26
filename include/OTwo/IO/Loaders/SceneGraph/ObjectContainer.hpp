#pragma once

#include <OTwo/States/State.hpp>

#include <Genode/IO/Resource.hpp>
#include <Genode/IO/ResourceContext.hpp>
#include <Genode/SceneGraph/Node.hpp>

#include <any>
#include <string>

class ObjectContainer
{
public:
    static ObjectContainer Decorate(State* container, bool importOnly = false);
    static ObjectContainer Decorate(Gx::Node* container);

    const std::string& GetName() const;

    template<typename R>
    void Add(const std::string& name, Gx::ResourcePtr<R> object, Gx::ResourceContext& ctx);

private:
    explicit ObjectContainer(Gx::Node* container, bool importOnly = false);

    Gx::Node* m_container;
    bool m_importOnly;
};

#include <OTwo/IO/Loaders/SceneGraph/ObjectContainer.inl>
