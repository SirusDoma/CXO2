namespace Gx
{
    template<typename T>
    T &Application::GetConfig()
    {
        static_assert(std::is_base_of_v<Config, T>, "Parameter must be a Gx::Config");

        if (auto it = m_configs.find(typeid(T)); it != m_configs.end())
            return static_cast<T&>(*it->second.get());

        if (auto it = m_configurators.find(typeid(T)); it != m_configurators.end())
        {
            auto configPtr = it->second(*this);
            if (configPtr)
                m_configs[typeid(T)] = std::move(configPtr);
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
    T &Application::Install()
    {
        static_assert(std::is_base_of_v<Module, T>, "Parameter must be a Gx::Module");

        if (auto iterator = m_modules.find(typeid(T)); iterator != m_modules.end() && iterator->second)
            return static_cast<T&>(*iterator->second.get());

        if (auto iterator = m_factories.find(typeid(T)); iterator != m_factories.end())
            m_modules[typeid(T)] = std::move(iterator->second(*this));

        // In case when the factory return nullptr
        if (!m_modules[typeid(T)])
            m_modules[typeid(T)] = std::move(std::make_unique<T>());

        return static_cast<T&>(*m_modules[typeid(T)].get());
    }

    template<typename T>
    bool Application::Provide(std::function<std::unique_ptr<T>(Application&)> builder)
    {
        static_assert(std::is_base_of_v<Module, T>, "Parameter must be a Gx::Module");

        if (auto iterator = m_modules.find(typeid(T)); iterator != m_modules.end())
            return false;

        m_factories[typeid(T)] = builder;
        return true;
    }

    template<typename T>
    bool Application::Uninstall()
    {
        static_assert(std::is_base_of_v<Module, T>, "Parameter must be a Gx::Module");

        if (auto iterator = m_modules.find(typeid(T)); iterator != m_modules.end())
            return m_modules.erase(iterator) == m_modules.end();

        return false;
    }

    template<typename T>
    T &Application::Require()
    {
        static_assert(std::is_base_of_v<Module, T>, "Parameter must be a Gx::Module");

        if (auto iterator = m_modules.find(typeid(T)); iterator != m_modules.end())
            return static_cast<T&>(*iterator->second.get());

        return Install<T>();
    }
}