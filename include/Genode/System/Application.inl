namespace Gx
{
    template<class T>
    bool Application::Install()
    {
        static_assert(std::is_base_of<Module, T>::value, "Parameter must be a Gx::Module");

        T* target = GetModule<T>();
        if (!target)
        {
            target = new T();
            m_modules.push_back(target);

            return true;
        }

        return false;
    }

    template<class T>
    bool Application::Install(T* instance)
    {
        static_assert(std::is_base_of<Module, T>::value, "Parameter must be a Gx::Module");

        auto target = GetModule<T>();
        if (!target)
        {
            m_modules.push_back(static_cast<Module*>(instance));
            return true;
        }

        return false;
    }

    template<class T>
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

    template<class T>
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