#ifndef O2JAM_OBJECT_LOADER_HPP
#define O2JAM_OBJECT_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectPopulator.hpp>

#include <Genode/IO/Json.hpp>

class ObjectLoader
{
public:
    static void Load(const std::string &name, const Gx::Json &json, ObjectPopulator &populator, Gx::ResourceContext &ctx);

private:
    template<typename R> static Gx::ResourcePtr<R> LoadResource(const std::string &name, const Gx::Json &json, const Gx::ResourceContext &ctx);
};

#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.inl>
#endif
