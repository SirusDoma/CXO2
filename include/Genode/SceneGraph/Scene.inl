namespace Gx
{
    template<typename R>
    R* Scene::Create(const std::string &source, ResourceScope scope)
    {
        static_assert(std::is_base_of<Gx::Node, R>::value, "Parameter must be a Gx::Node");
        return CreateNode<R>(source, scope);
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
    R& Scene::RegisterLocalResource()
    {
        static_assert(std::is_base_of<ResourceManager, R>::value, "Parameter must be a Gx::ResourceManager");

        if (m_resources)
            return *m_resources;

        m_resources = std::make_unique<R>();
        return *m_resources;
    }
}