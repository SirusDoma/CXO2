#pragma once

#include <CXO2/IO/Loaders/SceneGraph/ObjectContainer.hpp>

#include <Genode/IO/Json.hpp>

namespace Cx
{
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

}

#include <CXO2/IO/Loaders/SceneGraph/ObjectLoader.inl>
