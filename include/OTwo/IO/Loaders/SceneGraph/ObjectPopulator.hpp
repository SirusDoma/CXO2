#ifndef O2JAM_OBJECT_POPULATOR_HPP
#define O2JAM_OBJECT_POPULATOR_HPP

#include <Genode/IO/Resource.hpp>
#include <Genode/IO/ResourceContext.hpp>
#include <Genode/SceneGraph/Node.hpp>

#include <any>
#include <string>

// TODO: We can probably turn this class into a template class so the populator doesn't have to be a Gx::Node

class ObjectPopulator
{
public:
    static ObjectPopulator Decorate(Gx::Node *populator, bool importOnly = false);

    template<typename R>
    void Populate(const std::string &name, Gx::ResourcePtr<R> object, Gx::ResourceContext &ctx);

private:
    ObjectPopulator(Gx::Node *populator, bool importOnly = false);

    Gx::Node *m_populator;
    bool m_importOnly;
};

#include <OTwo/IO/Loaders/SceneGraph/ObjectPopulator.inl>
#endif
