#pragma once

#include <Genode/IO/FileSystem/FileSystem.hpp>
#include <Genode/System/Application.hpp>
#include <Genode/Utilities/StringHelper.hpp>
#include <SFML/Audio/SoundSource.hpp>

#include <fmt/format.h>

namespace Cx
{
    template<typename R, std::enable_if_t<std::is_base_of_v<Gx::Node, R> || std::is_base_of_v<sf::SoundSource, R>, int>>
    R* State::Instantiate(const std::string& source, const ResourceScope scope)
    {
        if constexpr (std::is_base_of_v<Gx::Node, R>)
        {
            if (auto resource = FindChild<R>(source))
                return resource;
        }

        auto resources = m_resources.get();
        if (scope == ResourceScope::Shared)
            resources = &GetApplication().GetModule<Gx::Context>().Require<Gx::ResourceManager>();

        if (!resources)
            return nullptr;

        R* instance = nullptr;
        if (instance = resources->Find<R>(source); !instance)
        {
            if (instance = resources->Find<R>(fmt::format("{}/{}",  GetName(), source)); !instance)
            {
                if constexpr (std::is_base_of_v<Gx::Node, R>)
                {
                    if (instance = dynamic_cast<R*>(resources->Find<Gx::Node>(source)); !instance)
                    {
                        if (instance = dynamic_cast<R*>(resources->Find<Gx::Node>(fmt::format("{}/{}",  GetName(), source))); !instance)
                            instance = &resources->AddFromFile<R>(Gx::StringHelper::RemoveExtension(source), source);
                    }
                }
                else
                {
                    if (Gx::FileSystem::Contains(source))
                        instance = &resources->AddFromFile<R>(Gx::StringHelper::RemoveExtension(source), source);
                    else
                        instance = &resources->AddFromDeserializer<R>(Gx::StringHelper::RemoveExtension(source), [this] { return GetContext().Instantiate<R>(); });
                }
            }
        }

        if constexpr (std::is_base_of_v<Gx::Node, R> && !std::is_base_of_v<Gx::Dialog, R>)
            AddChild(*instance);

        return instance;
    }

    template<typename R, std::enable_if_t<std::is_base_of_v<Gx::Node, R>, int>>
    R* State::Instantiate(const R& prefab, const ResourceScope scope)
    {
        auto resources = m_tempResources.get();
        if (scope == ResourceScope::Shared)
            resources = &GetApplication().GetModule<Gx::Context>().Require<Gx::ResourceManager>();
        else if (scope == ResourceScope::Local)
            resources = m_resources.get();

        auto name     = fmt::format("{}/{}_{}", GetName(), prefab.GetName(), std::to_string(resources->Count<R>()));
        auto resource = Gx::ResourcePtr<R>(new R(prefab), [] (auto ptr) { delete ptr; });
        auto instance = resources->Store<R>(name, std::move(resource), Gx::CacheMode::None);

        if constexpr (!std::is_base_of_v<Gx::Dialog, R>)
            AddChild(instance);

        return &instance;
    }

    template<typename R, std::enable_if_t<std::is_base_of_v<Gx::Node, R>, int>>
    R* State::Import(Gx::ResourcePtr<R> resource, const ResourceScope scope)
    {
        if (const auto node = dynamic_cast<Gx::Node*>(resource.get()); node)
            return Import<R>(fmt::format("{}/{}", GetName(), node->GetName()), std::move(resource), scope);

        return nullptr;
    }

    template<typename R>
    R* State::Import(const std::string& id, Gx::ResourcePtr<R> resource, const ResourceScope scope)
    {
        if (!resource)
            return nullptr;

        auto resources = m_resources.get();
        if (scope == ResourceScope::Shared)
            resources = &GetContext().Require<Gx::ResourceManager>();

        if (!resources)
            return nullptr;

        auto& result = resources->Store(id, std::move(resource), Gx::CacheMode::Update);
        return &result;
    }

    template<typename R, class... Args, std::enable_if_t<!std::is_array_v<R>, int>>
    R& State::Create(Args&&... args)
    {
        auto resources = m_tempResources.get();
        auto name      = Gx::StringHelper::GetTypeName<R>();

        Gx::ResourcePtr<R> resource = std::make_unique<R>(std::forward<Args>(args)...);
        return resources->Store(fmt::format("{}/{}_{}", GetName(), name, std::to_string(resources->Count<R>())), std::move(resource), Gx::CacheMode::None);
    }

    template<typename R>
    R* State::Find(const std::string& id, const ResourceScope scope)
    {
        auto resources = m_resources.get();
        if (scope == ResourceScope::Shared)
            resources = &GetApplication().GetModule<Gx::Context>().Require<Gx::ResourceManager>();
        else if (scope == ResourceScope::Immediate)
            resources = m_tempResources.get();

        if (!resources)
            return nullptr;

        if (auto resource = resources->Find<R>(id))
            return resource;

        if (auto resource = resources->Find<R>(fmt::format("{}/{}", GetName(), id)))
            return resource;

        if constexpr (std::is_base_of_v<Gx::Node, R>)
        {
            if (auto resource = dynamic_cast<R*>(resources->Find<Gx::Node>(id)))
                return resource;

            if (auto resource = dynamic_cast<R*>(resources->Find<Gx::Node>(fmt::format("{}/{}", GetName(), id))))
                return resource;
        }

        return nullptr;
    }
}
