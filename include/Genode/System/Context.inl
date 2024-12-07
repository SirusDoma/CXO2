#pragma once

#include <Genode/System/Exception.hpp>

namespace Gx
{
    template<typename T>
    void Context::Provide(const Scope scope)
    {
        static_assert(Constructible<T>::value, "Use Provide<T>(Builder<T>, Scope) instead for interface or complex constructible type");
        Provide<T>(As<T>(), scope);
    }

    template<typename T>
    void Context::Provide(Builder<T> builder, const Scope scope)
    {
        const std::type_index type = typeid(T);
        auto factory = std::make_shared<Factory<T>>(builder, scope);

        m_instances[type] = std::make_shared<Instance<T>>(std::move(factory->Create(*this)), scope);
        m_factories[type] = std::move(factory);
    }

    template<typename T>
    Context::Builder<T> Context::As() const
    {
        return Context::Builder<T>([this] (const Context& ctx) -> std::unique_ptr<T>
        {
            if constexpr (std::is_default_constructible_v<T>)
                return std::make_unique<T>();
            else if constexpr (Constructible<T>::value)
                return std::apply([](auto&&... args){ return std::make_unique<T>(args...); }, ctx.BuildParameters<ConstructorDescriptor<T>>());
            else
                throw NotSupportedException(std::string(typeid(T).name()) + " is not constructible");
        });
    }

    template<typename T>
    std::enable_if_t<!std::is_pointer_v<T>, T&>
    Context::Require() const
    {
        if (auto instance = Require<T*>(); instance)
            return *instance;

        if constexpr (Constructible<T>::value)
        {
            const std::type_index type = typeid(T);
            auto factory = std::make_shared<Factory<T>>(As<T>(), Scope::Local);

            m_instances[type] = std::make_shared<Instance<T>>(std::move(factory->Create(*this)), Scope::Local);
            m_factories[type] = std::move(factory);

            return static_cast<T&>(*(static_cast<Instance<T>*>(m_instances[type].get()))->Handle.get());
        }
        else
            throw Exception(std::string(typeid(T).name()) + " is not constructible and not provided within the current context");
    }

    template<typename T>
    std::enable_if_t<std::is_pointer_v<T>, T>
    Context::Require() const
    {
        using R = std::remove_pointer_t<T>;
        const std::type_index type = typeid(R);

        if (const auto it = m_instances.find(type); it != m_instances.end())
            return static_cast<T>((static_cast<Instance<R>*>(it->second.get()))->Handle.get());

        if (const auto it = m_factories.find(type); it != m_factories.end())
        {
            auto factory      = static_cast<Factory<R>*>(it->second.get());
            m_instances[type] = std::make_shared<Instance<R>>(std::move(factory->Create(*this)), Scope::Local);

            return static_cast<T>((static_cast<Instance<R>*>(m_instances[type].get()))->Handle.get());
        }

        return nullptr;
    }

    template<typename T>
    std::unique_ptr<T> Context::Create() const
    {
        if constexpr (!std::is_default_constructible_v<T>)
        {
            const std::type_index type = typeid(T);
            if (const auto it = m_factories.find(type); it != m_factories.end())
                return static_cast<Factory<T>*>(it->second.get())->Create(*this);

            auto factory = std::make_unique<Factory<T>>(As<T>(), Scope::Local);
            return factory->Create(*this);
        }
        else
            return std::make_unique<T>();
    }

    template<typename T, typename... Args>
    T Context::Invoke(std::function<T(Args&&...)> func) const
    {
        static_assert((Constructible<std::decay_t<Args>>::value && ...), "Function parameters cannot be constructed");

        return std::apply([func](auto&&... args)
        {
            return func(std::forward<Args>(args)...);
        }, BuildParameters<std::tuple<std::decay_t<Args>...>>());
    }

    template <typename T>
    decltype(auto) Context::BuildParameter() const
    {
        if constexpr (std::is_pointer_v<T>)
            return std::tuple { Require<T>() };
        else
            return std::tuple { std::tie(Require<T>()) };
    }

    template <typename Tuple, std::size_t... Is>
    auto Context::BuildParameters(std::index_sequence<Is...>) const
    {
        return std::tuple_cat(BuildParameter<std::tuple_element_t<Is, Tuple>>()...);
    }

    template <typename Tuple>
    auto Context::BuildParameters() const
    {
        constexpr std::size_t N = std::tuple_size_v<Tuple>;
        return BuildParameters<Tuple>(std::make_index_sequence<N>{});
    }
}
