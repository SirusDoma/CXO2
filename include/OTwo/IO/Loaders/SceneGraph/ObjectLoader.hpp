#pragma once

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectContainer.hpp>

#include <Genode/IO/Json.hpp>

class ObjectLoader
{
public:
    static void LoadFromJson(const std::string& name, const Gx::Json& json, ObjectContainer& container, Gx::ResourceContext& context);

private:
    template<typename R>
    static Gx::ResourcePtr<R> LoadFromJson(const std::string& name, const Gx::Json& json, const Gx::ResourceContext& context);

    template<typename R>
    static Gx::ResourcePtr<R> LoadFromJson(const std::string& name, const std::string& type, const Gx::Json& json, const Gx::ResourceContext& context);
};

#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.inl>
