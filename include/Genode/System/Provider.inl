#include <Genode/System/Exception.hpp>

namespace Gx
{
    template<typename T>
    void Provider::Provide(Builder<T> builder, const Scope scope)
    {
        const std::type_index type = typeid(T);
        std::unique_ptr<Factory<T>> factory = std::make_unique<Factory<T>>(builder, scope);

        m_instances[type] = std::make_unique<Instance<T>>(std::move(factory->Builder(*this)), scope);
        m_factories[type] = std::move(factory);
    }

    template<typename T>
    Provider::Builder<T> Provider::As()
    {
        return Provider::Builder<T>([this] (Provider& ctx) -> std::unique_ptr<T>
        {
            if constexpr(Gx::GetConstructorParameterCount<T>(0) == 0)
                return std::make_unique<T>();
            else
                return std::make_unique<T>(std::make_from_tuple<T>(ctx.BuildParameters<ConstructorDescriptor<T>>()));
        });
    }

    template<typename T>
    std::enable_if_t<!std::is_pointer_v<T>, T&>
    Provider::Require()
    {
        if (auto instance = Require<T*>(); instance)
            return *instance;

        throw Exception(std::string(typeid(T).name()) + " is not constructible and not provided within the current context");
    }

    template<typename T>
    std::enable_if_t<std::is_pointer_v<T>, T>
    Provider::Require()
    {
        using R = std::remove_pointer_t<T>;
        const std::type_index type = typeid(R);

        if (const auto it = m_instances.find(type); it != m_instances.end())
            return static_cast<T>((static_cast<Instance<R>*>(it->second.get()))->Handle.get());

        if (const auto it = m_factories.find(type); it != m_factories.end())
        {
            auto factory      = static_cast<Factory<R>*>(it->second.get());
            auto instance     = std::make_unique<Instance<R>>(std::move(factory->Builder(*this)), Scope::Local);
            m_instances[type] = std::move(instance);

            return static_cast<T>((static_cast<Instance<R>*>(m_instances[type].get()))->Handle.get());
        }

        if constexpr (Constructible<R>::value)
        {
            Provide<R>();
            return Require<T>();
        }
        else
            return nullptr;
    }

    template <typename T>
    decltype(auto) Provider::BuildParameter()
    {
        if constexpr (std::is_pointer_v<T>)
            return std::tuple { Require<T>() };
        else
            return std::tuple { std::tie(Require<T>()) };
    }

    template <typename Tuple, std::size_t... Is>
    auto Provider::BuildParameters(std::index_sequence<Is...>)
    {
        return std::tuple_cat(BuildParameter<std::tuple_element_t<Is, Tuple>>()...);
    }

    template <typename Tuple>
    auto Provider::BuildParameters()
    {
        constexpr std::size_t N = std::tuple_size_v<Tuple>;
        return BuildParameters<Tuple>(std::make_index_sequence<N>{});
    }

    template<typename T>
    void Provider::Provide(const Scope scope)
    {
        static_assert(Constructible<T>::value, "Use Provide<T>(Builder<T>, Scope) instead for interface or complex constructible type");
        Provide<T>(As<T>(), scope);
    }
}
