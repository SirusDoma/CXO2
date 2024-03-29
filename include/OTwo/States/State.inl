#include <Genode/Utilities/StringHelper.hpp>

template<typename R>
R* State::Load(const std::string &source, ResourceScope scope)
{
    static_assert(
        std::is_base_of_v<Gx::Node, R> ||
        std::is_base_of_v<sf::SoundSource, R>,
        "Parameter must be a Gx::Node or sf::SoundSource"
    );

    if constexpr (std::is_base_of_v<Gx::Node, R>)
    {
        auto resource = FindChild<R>(source);
        if (resource)
            return resource;
    }

    auto resources = m_resources.get();
    if (scope == ResourceScope::Shared)
        resources = &Require<Gx::ResourceManager>();

    if (!resources)
        return nullptr;

    R* resource;
    if (resource = resources->Find<R>(source); !resource)
    {
        if (resource = resources->Find<R>(GetName() + "/" + source); !resource)
            resource = &resources->AddFromFile<R>(Gx::StringHelper::RemoveExtension(source), source);
    }

    if constexpr (std::is_base_of_v<Gx::Node, R> && !std::is_base_of_v<Gx::Dialog, R>)
        AddChild(resource);

    return resource;
}

template<typename R>
R *State::Import(const std::string &id, Gx::ResourcePtr<R> resource, ResourceScope scope)
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
    if constexpr (std::is_base_of_v<Gx::Node, R> && !std::is_base_of_v<Gx::Dialog, R>)
        AddChild(&result);

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

template<typename R, class... Args, std::enable_if_t<!std::is_array_v<R>, int>>
R *State::Make(const std::string &id, Args &&... args)
{
    auto resources = m_tempResources.get();
    if (!resources)
        return nullptr;

    auto resource = Gx::ResourcePtr<R>(new R(std::forward<Args>(args)...), [] (auto ptr) { delete ptr; });
    R* child = &resources->Store(id, std::move(resource));

    if constexpr (std::is_base_of_v<Gx::Node, R> && !std::is_base_of_v<Gx::Dialog, R>)
        child->SetName(id);

    return child;
}

template<typename R>
R *State::Instantiate(const std::string &source, ResourceScope scope)
{
    static_assert(
            std::is_base_of_v<Gx::Node, R> ||
            std::is_base_of_v<sf::SoundSource, R>,
            "Parameter must be a Gx::Node or sf::SoundSource"
    );

    auto resources = m_resources.get();
    if (scope == ResourceScope::Shared)
        resources = &Require<Gx::ResourceManager>();

    if (!resources)
        return nullptr;

    auto name     = Gx::StringHelper::GetTypeName<R>();
    auto resource = &resources->AddFromFile<R>(GetName() + "/" + name + "_" + std::to_string(resources->Count<R>()), source);

    if constexpr (std::is_base_of_v<Gx::Node, R> && !std::is_base_of_v<Gx::Dialog, R>)
        AddChild(resource);

    return resource;
}


template<typename R, typename T>
R *State::Instantiate(const std::string &id, ResourceScope scope)
{
    static_assert(
        std::is_base_of_v<Gx::Node, R> ||
        std::is_base_of_v<sf::SoundSource, R> ||
        std::is_base_of_v<Gx::Node, T> ||
        std::is_base_of_v<sf::SoundSource, T>,
        "Parameter must be a Gx::Node or sf::SoundSource"
    );

    auto prefab = Load<T>(id, scope);
    if (!prefab)
        return nullptr;

    auto resources = m_resources.get();
    if (scope == ResourceScope::Shared)
        resources = &Require<Gx::ResourceManager>();

    if (!resources)
        return nullptr;

    if constexpr (std::is_base_of_v<Gx::Node, R> && !std::is_base_of_v<Gx::Dialog, R>)
        RemoveChild(prefab);

    auto name = id;
    if (!resources->Find<T>(name))
        name = GetName() + "/" + id;

    auto resource = Create<R>(std::move(*prefab));
    resources->Destroy<T>(name);

    if constexpr (std::is_base_of_v<Gx::Node, R> && !std::is_base_of_v<Gx::Dialog, R>)
        AddChild(resource);

    return resource;
}

template<typename R>
R *State::Instantiate(const std::string &source, const std::string &id, ResourceScope scope)
{
    static_assert(
        std::is_base_of_v<Gx::Node, R> ||
        std::is_base_of_v<sf::SoundSource, R>,
        "Parameter must be a Gx::Node or sf::SoundSource"
    );

    auto resource = Find<R>(source, scope);
    return Make<R>(id, *resource);
}


template<typename R>
R *State::Find(const std::string &id, ResourceScope scope)
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

    return resources->Find<R>(id);
}


