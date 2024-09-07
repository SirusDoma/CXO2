namespace Gx
{
    template<typename T>
    template<class... Args>
    std::unique_ptr<T> ResourceLoader<T>::Create(Args&&... args) const
    {
        return m_creator->Build(args...);
    };

    template<typename T>
    template<typename R, typename... Args>
    void ResourceLoader<T>::SetResourceCreator(const ResourceCreator<R, Args...>& builder)
    {
        static_assert(std::is_base_of_v<T, R>, "Specified type must be a derived class of the base resource type");

        m_type    = std::type_index(typeid(R));
        m_creator = std::make_unique<ResourceBuilder<Args...>>(m_type, [=] (Args&&... args) { return builder(args...); });
    }

    template<typename T>
    template<typename... Args>
    std::unique_ptr<T> ResourceLoader<T>::ResourceBuilderBase::Build(Args&&... args) const
    {
        if (auto creator = dynamic_cast<const ResourceBuilder<Args...>*>(this); creator)
            return creator->Create(args...);

        throw ArgumentException("The specified arguments cannot be used to construct the " + std::string(Type.name()));
    }
}