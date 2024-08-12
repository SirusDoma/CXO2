#include <Genode/Utilities/StringHelper.hpp>

template<typename R>
R* State::Instantiate(const std::string &source, const ResourceScope scope)
{
    static_assert(
        std::is_base_of_v<Gx::Node, R> ||
        std::is_base_of_v<sf::SoundSource, R>,
        "Parameter must be a Gx::Node or sf::SoundSource"
    );

    if constexpr (std::is_base_of_v<Gx::Node, R>)
    {
        if (auto resource = FindChild<R>(source))
            return resource;
    }

    auto resources = m_resources.get();
    if (scope == ResourceScope::Shared)
        resources = &Require<Gx::ResourceManager>();

    if (!resources)
        return nullptr;

    R* instance = nullptr;
    if (instance = resources->Find<R>(source); !instance)
    {
        if (instance = resources->Find<R>(GetName() + "/" + source); !instance)
            instance = &resources->AddFromFile<R>(Gx::StringHelper::RemoveExtension(source), source);
    }

    if constexpr (std::is_base_of_v<Gx::Node, R> && !std::is_base_of_v<Gx::Dialog, R>)
        AddChild(instance);

    return instance;
}

template<typename R>
R* State::Instantiate(const R& prefab, const ResourceScope scope)
{
    static_assert(
        std::is_base_of_v<Gx::Node, R>,
        "Parameter must be a Gx::Node"
    );

    auto resources = m_tempResources.get();
    if (scope == ResourceScope::Shared)
        resources = &Require<Gx::ResourceManager>();
    else if (scope == ResourceScope::Local)
        resources = m_resources.get();

    auto name     = GetName() + "/" + prefab.GetName() + "_" + std::to_string(resources->Count<R>());
    auto resource = Gx::ResourcePtr<R>(new R(prefab), [] (auto ptr) { delete ptr; });
    auto instance = &resources->Store<R>(name, std::move(resource), Gx::CacheMode::None);

    if constexpr (!std::is_base_of_v<Gx::Dialog, R>)
        AddChild(instance);

    return instance;
}

template<typename R>
R *State::Import(Gx::ResourcePtr<R> resource, const ResourceScope scope)
{
    static_assert(
        std::is_base_of_v<Gx::Node, R>,
        "Parameter must be a Gx::Node"
    );

    if (const auto node = dynamic_cast<Gx::Node*>(resource.get()); node)
        return Import<R>(GetName() + "/" + node->GetName(), std::move(resource), scope);

    return nullptr;
}

template<typename R>
R *State::Import(const std::string &id, Gx::ResourcePtr<R> resource, const ResourceScope scope)
{
    static_assert(
        std::is_base_of_v<Gx::Node, R> ||
        std::is_base_of_v<sf::SoundSource, R>,
        "Parameter must be a Gx::Node or sf::SoundSource"
    );

    if (!resource)
        return nullptr;

    auto resources = m_resources.get();
    if (scope == ResourceScope::Shared)
        resources = &Require<Gx::ResourceManager>();

    if (!resources)
        return nullptr;

    auto& result = resources->Store(id, std::move(resource), Gx::CacheMode::Update);
    return &result;
}

template<typename R, class... Args, std::enable_if_t<!std::is_array_v<R>, int>>
R *State::Create(Args&&... args)
{
    auto resources = m_tempResources.get();
    if (!resources)
        return nullptr;

    auto name     = Gx::StringHelper::GetTypeName<R>();
    auto resource = Gx::ResourcePtr<R>(new R(std::forward<Args>(args)...), [] (auto ptr) {
        delete ptr;
    });

    return &resources->Store(GetName() + "/" + name + "_" + std::to_string(resources->Count<R>()), std::move(resource), Gx::CacheMode::None);
}

template<typename R>
R *State::FindResource(const std::string &id, const ResourceScope scope)
{
    static_assert(
        std::is_base_of_v<Gx::Node, R> ||
        std::is_base_of_v<sf::SoundSource, R>,
        "Parameter must be a Gx::Node or sf::SoundSource"
    );

    auto resources = m_resources.get();
    if (scope == ResourceScope::Shared)
        resources = &Require<Gx::ResourceManager>();
    else if (scope == ResourceScope::Immediate)
        resources = m_tempResources.get();

    if (!resources)
        return nullptr;

    if (auto resource = resources->Find<R>(id); resource)
        return resource;

    return resources->Find<R>(GetName() + "/" + id);
}


