namespace Gx
{
    template<typename R>
    R* Scene::Create(const std::string &source, ResourceScope scope)
    {
        static_assert(std::is_base_of<Gx::Node, R>::value, "Parameter must be a Gx::Node");

        auto resources = m_resources.get();
        if (scope == ResourceScope::Shared)
            resources = &GetApplication().Require<Gx::ResourceManager>();

        if (!resources)
            return nullptr;

        auto resource = resources->Resolve<R>(source);
        auto deleter  = resource.get_deleter();
        auto entity   = resource.release();

        m_entities.push_back(ResourcePtr<Node>{dynamic_cast<Node*>(entity), [deleter] (auto node) { deleter(dynamic_cast<R*>(node)); }});
        return dynamic_cast<R*>(entity);
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