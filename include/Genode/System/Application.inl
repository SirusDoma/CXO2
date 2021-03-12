namespace Gx
{
    template<typename T>
    bool Application::Install()
    {
        static_assert(std::is_base_of<Module, T>::value, "Parameter must be a Gx::Module");
        return Install(new T());
    }

    template<typename T>
    bool Application::Install(T* mod)
    {
        static_assert(std::is_base_of<Module, T>::value, "Parameter must be a Gx::Module");

        auto target = GetModule<T>();
        if (!target)
        {
            m_modules.push_back(static_cast<Module*>(mod));
            return true;
        }

        return false;
    }

    template<typename T>
    bool Application::Uninstall()
    {
        static_assert(std::is_base_of<Module, T>::value, "Parameter must be a Gx::Module");

        for (size_t i = 0; i < m_modules.size(); i++)
        {
            T* target = dynamic_cast<T*>(m_modules.at(i));
            if (target)
            {
                m_modules.erase(m_modules.begin() + i);

                delete target;
                return true;
            }
        }

        return false;
    }

    template<typename T>
    T* Application::GetModule() const
    {
        static_assert(std::is_base_of<Module, T>::value, "Parameter must be a Gx::Module");
        for (auto mod : m_modules)
        {
            auto target = dynamic_cast<T*>(mod);
            if (target)
            {
                return target;
            }
        }

        return nullptr;
    }
}