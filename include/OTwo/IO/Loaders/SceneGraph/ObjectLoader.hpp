#ifndef O2JAM_OBJECT_LOADER_HPP
#define O2JAM_OBJECT_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectContainer.hpp>

#include <Genode/IO/Json.hpp>

class ObjectLoader
{
public:
    static void Load(const std::string& name, const Gx::Json& json, ObjectContainer& container, Gx::ResourceContext& ctx);

private:
    template<typename R>
    static Gx::ResourcePtr<R> LoadResource(const std::string& name, const Gx::Json& json, const Gx::ResourceContext& context);

    template<typename B, typename R>
    static Gx::ResourcePtr<B> LoadResource(const std::string& name, const Gx::Json& json, const Gx::ResourceContext& context);
};

#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.inl>
#endif
