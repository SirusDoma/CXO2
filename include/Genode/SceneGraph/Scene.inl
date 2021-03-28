namespace Gx
{
    template<typename R>
    R* Scene::Create(const std::string &source, ResourceScope scope)
    {
        static_assert(std::is_base_of<Node, R>::value || std::is_base_of<sf::SoundSource, R>::value, "Parameter must be a Gx::Node or sf::SoundSource");

        if constexpr (std::is_base_of<sf::SoundSource, R>::value)
            return CreateSoundSource<R>(source, scope);
        else if constexpr (std::is_base_of<Gx::Node, R>::value)
            return CreateNode<R>(source, scope);

        return nullptr;
    }

    template<typename R>
    R *Scene::CreateNode(const std::string &source, ResourceScope scope)
    {
        static_assert(std::is_base_of<Gx::Node, R>::value, "Parameter must be a Gx::Node");

        auto resources = m_resources.get();
        if (scope == ResourceScope::Shared)
            resources = &GetSharedResources();

        if (!resources)
            return nullptr;

        auto resource = resources->Resolve<R>(source);
        auto deleter  = resource.get_deleter();

        m_entities.push_back(ResourcePtr<Node>{dynamic_cast<Node*>(resource.release()), [deleter] (auto node) { deleter(dynamic_cast<R*>(node)); }});
        return dynamic_cast<R*>(m_entities.back().get());
    }

    template<typename R>
    R *Scene::CreateSoundSource(const std::string &source, ResourceScope scope)
    {
        static_assert(std::is_base_of<sf::SoundSource, R>::value, "Parameter must be a sf::SoundSource");

        auto resources = m_resources.get();
        if (scope == ResourceScope::Shared)
            resources = &GetSharedResources();

        if (!resources)
            return nullptr;

        auto resource = resources->Resolve<R>(source);
        auto deleter  = resource.get_deleter();

        m_sources.push_back(ResourcePtr<sf::SoundSource>{dynamic_cast<sf::SoundSource*>(resource.release()), [deleter] (auto sound) { deleter(dynamic_cast<R*>(sound)); }});
        return dynamic_cast<R*>(m_sources.back().get());
    }

    template<typename R>
    R& Scene::RegisterLocalResource()
    {
        static_assert(std::is_base_of<ResourceManager, R>::value, "Parameter must be a Gx::ResourceManager");

        if (m_resources)
            return *m_resources;

        m_resources = std::make_unique<R>();
        return *m_resources;
    }
}