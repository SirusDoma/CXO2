#pragma once

#include <Genode/System/Exception.hpp>

#include <memory>
#include <functional>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <type_traits>
#include <stdexcept>
#include <string>

namespace Gx
{
    class Context
    {
        static constexpr std::size_t MaxConstructorArity = 100;

        template <typename Exclude>
        struct AnyType
        {
            template <typename T, typename = std::enable_if_t<
                !std::is_pointer_v<T> &&
                !std::is_same_v<std::decay_t<T>, std::decay_t<Exclude>> &&
                !std::is_same_v<std::decay_t<T>, Context>>>
            // ReSharper disable once CppFunctionIsNotImplemented CppNonExplicitConversionOperator
            operator T& () const noexcept;

            template <typename T, typename = std::enable_if_t<
                !std::is_pointer_v<T> &&
                !std::is_same_v<std::decay_t<T>, std::decay_t<Exclude>> &&
                !std::is_same_v<std::decay_t<T>, Context>>>
            // ReSharper disable once CppFunctionIsNotImplemented CppNonExplicitConversionOperator
            operator T&& () const noexcept;

            template <typename T, typename = std::enable_if_t<
                std::is_pointer_v<T> &&
                !std::is_same_v<std::decay_t<std::remove_pointer_t<T>>, std::decay_t<Exclude>> &&
                !std::is_same_v<std::decay_t<std::remove_pointer_t<T>>, Context>>, typename = void>
            // ReSharper disable once CppFunctionIsNotImplemented CppNonExplicitConversionOperator
            operator T () const noexcept;
        };

        template <typename T, typename... Probes>
        struct MinArity
            : std::conditional_t<
                  std::disjunction_v<
                      std::bool_constant<(sizeof...(Probes) > MaxConstructorArity)>,
                      std::is_constructible<T, Probes...>>,
                  std::integral_constant<std::size_t, sizeof...(Probes)>,
                  MinArity<T, Probes..., AnyType<T>>> {};

        template <typename T>
        static constexpr std::size_t MinArityV = MinArity<T>::value;

        template <typename T>
        static constexpr bool IsConstructible =
            !std::is_abstract_v<T> &&
            (MinArityV<T> <= MaxConstructorArity);

        template <typename Owner>
        struct Resolver
        {
            const Context& Ctx;

            template <typename T, typename = std::enable_if_t<
                !std::is_pointer_v<T> &&
                !std::is_same_v<std::decay_t<T>, std::decay_t<Owner>> &&
                !std::is_same_v<std::decay_t<T>, Context>>>
            // ReSharper disable once CppNonExplicitConversionOperator
            operator T& () const;

            template <typename T, typename = std::enable_if_t<
                !std::is_pointer_v<T> &&
                !std::is_same_v<std::decay_t<T>, std::decay_t<Owner>> &&
                !std::is_same_v<std::decay_t<T>, Context>>>
            // ReSharper disable once CppNonExplicitConversionOperator
            operator T&& () const;

            template <typename T, typename = std::enable_if_t<
                std::is_pointer_v<T> &&
                !std::is_same_v<std::decay_t<std::remove_pointer_t<T>>, std::decay_t<Owner>> &&
                !std::is_same_v<std::decay_t<std::remove_pointer_t<T>>, Context>>, typename = void>
            // ReSharper disable once CppNonExplicitConversionOperator
            operator T () const;
        };

    public:
        enum class Scope
        {
            Local,
            Singleton
        };

        Context() : m_parent(nullptr) {}

        Context(const Context&)            = delete;
        Context& operator=(const Context&) = delete;

        Context(Context&& other) noexcept
            : m_parent(other.m_parent),
              m_entries(std::move(other.m_entries))
        {}

        Context& operator=(Context&& other) noexcept
        {
            if (this != &other)
            {
                m_parent  = other.m_parent;
                m_entries = std::move(other.m_entries);
                m_current = nullptr;
            }
            return *this;
        }

        template <typename T, std::enable_if_t<IsConstructible<T>, int> = 0>
        void Provide(Scope scope = Scope::Local);

        template <typename TInterface, typename TConcrete,
            std::enable_if_t<
                std::is_base_of_v<TInterface, TConcrete> &&
                IsConstructible<TConcrete>, int
            > = 0
        >
        void Provide(Scope scope = Scope::Local);

        template <typename T>
        void Provide(std::function<std::unique_ptr<T>(const Context&)> builder,
                     Scope scope = Scope::Local);

        template <typename T>
        std::enable_if_t<!std::is_pointer_v<T>, T&>
        Require() const;

        template <typename T>
        std::enable_if_t<std::is_pointer_v<T>, T>
        Require() const;

        template <typename T>
        std::unique_ptr<T> Instantiate() const;

        Context CreateScope()
        {
            Context scope;
            scope.m_parent = this;
            for (auto& [key, entry] : m_entries)
            {
                if (entry->Lifetime == Scope::Singleton)
                    scope.m_entries[key] = entry;
                else
                    scope.m_entries[key] = entry->Clone(true);
            }
            return scope;
        }

        Context Capture() const
        {
            Context captured;
            captured.m_parent = nullptr;

            for (auto& [key, entry] : m_entries)
                captured.m_entries[key] = entry->Clone(false);

            return captured;
        }

    private:
        struct Scopable
        {
            Scope Lifetime = Scope::Local;
            std::vector<std::shared_ptr<Scopable>> Dependencies;
            virtual ~Scopable() = default;
            [[nodiscard]] virtual std::shared_ptr<Scopable> Clone(bool reset) const = 0;
        };

        template <typename T>
        struct Service final : Scopable
        {
            std::function<std::unique_ptr<T>(const Context&)> Builder;
            std::shared_ptr<T> Instance;

            [[nodiscard]] std::shared_ptr<Scopable> Clone(bool reset) const override;
        };

        using ScopableMap = std::unordered_map<std::type_index, std::shared_ptr<Scopable>>;

        template <typename T, std::size_t... Is>
        static std::unique_ptr<T> Construct(const Context& c, std::index_sequence<Is...>);

        template <typename T>
        static std::function<std::unique_ptr<T>(const Context&)> CreateBuilder();

        template <typename T>
        Service<T>* GetService(std::type_index key) const;

        template <typename T>
        void ProvideDefault(std::type_index key) const;

        Context*            m_parent;
        mutable ScopableMap m_entries;
        mutable Scopable*   m_current = nullptr;
    };
}

#include <Genode/System/Context.inl>
