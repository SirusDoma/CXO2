namespace Gx
{
    template<typename T>
    T &Application::GetConfig()
    {
        static_assert(std::is_base_of<Config, T>::value, "Parameter must be a Gx::Config");

        if (auto it = m_configs.find(typeid(T)); it != m_configs.end())
            return static_cast<T&>(*it->second.get());

        if (auto it = m_configResolvers.find(typeid(T)); it != m_configResolvers.end())
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
        static_assert(std::is_base_of<Config, T>::value, "Parameter must be a Gx::Config");

        m_configs[typeid(T)] = std::make_unique<T>(config);
    }

    template<typename T>
    void Application::SetConfig(std::function<std::unique_ptr<T>(const Application&)> resolver)
    {
        static_assert(std::is_base_of<Config, T>::value, "Parameter must be a Gx::Config");

        m_configResolvers[typeid(T)] = resolver;
    }

    template<typename T>
    T &Application::Install()
    {
        static_assert(std::is_base_of<Module, T>::value, "Parameter must be a Gx::Module");

        if (auto iterator = m_modules.find(typeid(T)); iterator != m_modules.end())
            return static_cast<T&>(*iterator->second.get());

        auto iterator = m_moduleResolvers.find(typeid(T));
        bool managed  = iterator != m_moduleResolvers.end();
        auto instance = managed ? &iterator->second(*this) : new T();
        m_moduleResolvers.erase(iterator);

        auto [module, _] = m_modules.insert({typeid(T), ResourcePtr<Module>{instance, [managed] (auto ptr)
        {
            if (!managed)
                delete ptr;
        }}});

        return static_cast<T&>(*module->second.get());
    }

    template<typename T>
    bool Application::Resolve(std::function<T&(Application&)> resolver)
    {
        static_assert(std::is_base_of<Module, T>::value, "Parameter must be a Gx::Module");

        if (auto iterator = m_modules.find(typeid(T)); iterator != m_modules.end())
            return false;

        m_moduleResolvers[typeid(T)] = resolver;
        return true;
    }

    template<typename T>
    bool Application::Uninstall()
    {
        static_assert(std::is_base_of<Module, T>::value, "Parameter must be a Gx::Module");

        if (auto iterator = m_modules.find(typeid(T)); iterator != m_modules.end())
            return m_modules.erase(iterator) == m_modules.end();

        return false;
    }

    template<typename T>
    T &Application::Require()
    {
        static_assert(std::is_base_of<Module, T>::value, "Parameter must be a Gx::Module");

        if (auto iterator = m_modules.find(typeid(T)); iterator != m_modules.end())
            return static_cast<T&>(*iterator->second.get());

        return Install<T>();
    }
}