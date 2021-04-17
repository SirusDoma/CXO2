namespace Gx
{
    template<typename T>
    T *Application::Install()
    {
        static_assert(std::is_base_of<Module, T>::value, "Parameter must be a Gx::Module");
        return Install(std::make_unique<T>());
    }

    template<typename T>
    T *Application::Install(T *instance)
    {
        static_assert(std::is_base_of<Module, T>::value, "Parameter must be a Gx::Module");

        auto target = GetModule<T>();
        if (!target)
        {
            auto ptr = std::unique_ptr<Module>(static_cast<Module*>(instance));
            auto mod = ptr.get();
            m_modules.push_back(std::move(ptr));

            return dynamic_cast<T*>(mod);
        }

        return nullptr;
    }

    template<typename T>
    bool Application::Uninstall()
    {
        static_assert(std::is_base_of<Module, T>::value, "Parameter must be a Gx::Module");

        for (size_t i = 0; i < m_modules.size(); i++)
        {
            T* target = dynamic_cast<T*>(m_modules.at(i).get());
            if (target)
            {
                m_modules.erase(m_modules.begin() + i);
                return true;
            }
        }

        return false;
    }

    template<typename T>
    T* Application::GetModule() const
    {
        static_assert(std::is_base_of<Module, T>::value, "Parameter must be a Gx::Module");
        for (auto &mod : m_modules)
        {
            auto target = dynamic_cast<T*>(mod.get());
            if (target)
            {
                return target;
            }
        }

        return nullptr;
    }
}