namespace Gx
{
    template<typename T>
    T &Application::Provide()
    {
        static_assert(std::is_base_of_v<Provider, T>, "Parameter must be a Gx::Provider");

        if (const auto iterator = m_providers.find(typeid(T)); iterator != m_providers.end() && iterator->second)
            return static_cast<T&>(*iterator->second.get());

        if (const auto iterator = m_factories.find(typeid(T)); iterator != m_factories.end())
            m_providers[typeid(T)] = std::move(iterator->second(*this));

        // In case when the factory return nullptr
        if (!m_providers[typeid(T)])
            m_providers[typeid(T)] = std::move(std::make_unique<T>());

        return static_cast<T&>(*m_providers[typeid(T)].get());
    }

    template<typename T>
    bool Application::Provide(std::function<std::unique_ptr<T>(Application&)> builder)
    {
        static_assert(std::is_base_of_v<Provider, T>, "Parameter must be a Gx::Provider");

        Uninstall<T>();

        m_factories[typeid(T)] = builder;
        return true;
    }

    template<typename T>
    bool Application::Uninstall()
    {
        static_assert(std::is_base_of_v<Provider, T>, "Parameter must be a Gx::Provider");

        if (const auto iterator = m_providers.find(typeid(T)); iterator != m_providers.end())
            return m_providers.erase(iterator) == m_providers.end();

        return false;
    }

    template<typename T>
    T &Application::Require()
    {
        static_assert(std::is_base_of_v<Provider, T>, "Parameter must be a Gx::Provider");

        if (const auto iterator = m_providers.find(typeid(T)); iterator != m_providers.end())
            return static_cast<T&>(*iterator->second.get());

        return Provide<T>();
    }
}