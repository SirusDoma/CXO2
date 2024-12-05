#pragma once

namespace Gx
{
    template<typename T>
    template<class... Args>
    std::unique_ptr<T> ResourceLoader<T>::Instantiate(const ResourceContext& context, Args&&... args) const
    {
        if constexpr (std::is_default_constructible_v<T>)
        {
            if (!m_creator)
                return std::make_unique<T>();
        }

        if (!m_creator)
            throw NotSupportedException("Insufficient information to construct the resource of " + std::string(m_type.name()));

        return m_creator->Build(context, args...);
    };

    template<typename T>
    template<typename R, typename... Args>
    void ResourceLoader<T>::SetResourceInstantiator(const ResourceInstantiator<R, Args...>& builder)
    {
        static_assert(std::is_base_of_v<T, R>, "Specified type must be a derived class of the base resource type");

        m_type    = std::type_index(typeid(R));
        m_creator = std::make_unique<ResourceBuilder<Args...>>(m_type, [=] (const ResourceContext& context, Args&&... args)
        {
            return builder(context, args...);
        });
    }

    template<typename T>
    template<typename... Args>
    std::unique_ptr<T> ResourceLoader<T>::ResourceBuilderBase::Build(const ResourceContext& context, Args&&... args) const
    {
        if (auto builder = dynamic_cast<const ResourceBuilder<Args...>*>(this))
            return builder->Instantiate(context, args...);

        throw ArgumentException("The specified arguments cannot be used to construct the " + std::string(Type.name()));
    }
}