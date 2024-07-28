namespace Gx
{
    template<typename T>
    T &Application::GetConfig()
    {
        static_assert(std::is_base_of_v<Config, T>, "Parameter must be a Gx::Config");

        if (const auto it = m_configs.find(typeid(T)); it != m_configs.end())
            return static_cast<T&>(*it->second.get());

        if (const auto it = m_configurators.find(typeid(T)); it != m_configurators.end())
        {
            if (auto config = it->second(*this))
                m_configs[typeid(T)] = std::move(config);
            else
                m_configs[typeid(T)] = std::make_unique<T>();
        }
        else
            m_configs[typeid(T)] = std::make_unique<T>();

        return static_cast<T&>(*m_configs[typeid(T)].get());
    }

    template<typename T>
    void Application::SetConfig(const T &config)
    {
        static_assert(std::is_base_of_v<Config, T>, "Parameter must be a Gx::Config");

        m_configs[typeid(T)] = std::make_unique<T>(config);
    }

    template<typename T>
    void Application::SetConfig(std::function<std::unique_ptr<T>(const Application&)> builder)
    {
        static_assert(std::is_base_of_v<Config, T>, "Parameter must be a Gx::Config");

        m_configurators[typeid(T)] = builder;
    }

    template<typename T>
    T &Application::Provide()
    {
        static_assert(std::is_base_of_v<Context, T>, "Parameter must be a Gx::Context");

        if (const auto iterator = m_contexts.find(typeid(T)); iterator != m_contexts.end() && iterator->second)
            return static_cast<T&>(*iterator->second.get());

        if (const auto iterator = m_factories.find(typeid(T)); iterator != m_factories.end())
            m_contexts[typeid(T)] = std::move(iterator->second(*this));

        // In case when the factory return nullptr
        if (!m_contexts[typeid(T)])
            m_contexts[typeid(T)] = std::move(std::make_unique<T>());

        return static_cast<T&>(*m_contexts[typeid(T)].get());
    }

    template<typename T>
    bool Application::Provide(std::function<std::unique_ptr<T>(Application&)> builder)
    {
        static_assert(std::is_base_of_v<Context, T>, "Parameter must be a Gx::Context");

        if (const auto iterator = m_contexts.find(typeid(T)); iterator != m_contexts.end())
            return false;

        m_factories[typeid(T)] = builder;
        return true;
    }

    template<typename T>
    bool Application::Uninstall()
    {
        static_assert(std::is_base_of_v<Context, T>, "Parameter must be a Gx::Context");

        if (const auto iterator = m_contexts.find(typeid(T)); iterator != m_contexts.end())
            return m_contexts.erase(iterator) == m_contexts.end();

        return false;
    }

    template<typename T>
    T &Application::Require()
    {
        static_assert(std::is_base_of_v<Context, T>, "Parameter must be a Gx::Context");

        if (const auto iterator = m_contexts.find(typeid(T)); iterator != m_contexts.end())
            return static_cast<T&>(*iterator->second.get());

        return Provide<T>();
    }
}