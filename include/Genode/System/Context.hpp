#pragma once

#include <Genode/Utilities/Reflection.hpp>

#include <functional>
#include <typeindex>
#include <unordered_map>
#include <memory>
#include <type_traits>

namespace Gx
{
    class Context final
    {
    public:
        template<typename T>
        using Builder = std::function<std::unique_ptr<T>(const Context&)>;

        enum class Scope
        {
            Local,
            Shared
        };

        Context() = default;
        Context(Context&& other) noexcept
        {
            m_factories = std::exchange(other.m_factories, {});
            m_instances = std::exchange(other.m_instances, {});
        }

        virtual ~Context() = default;

        Context& operator=(Context&& other) noexcept
        {
            m_factories = std::exchange(other.m_factories, {});
            m_instances = std::exchange(other.m_instances, {});

            return *this;
        }

        template<typename T>
        void Provide(Scope scope = Scope::Local);

        template<typename T>
        void Provide(Builder<T> builder, Scope scope = Scope::Local);

        template<typename T>
        Builder<T> As() const;

        template<typename T>
        std::enable_if_t<!std::is_pointer_v<T>, T&> Require() const;

        template<typename T>
        std::enable_if_t<std::is_pointer_v<T>, T> Require() const;

        template<typename T>
        std::unique_ptr<T> Create() const;

        template<typename T, typename... Args>
        T Invoke(std::function<T(Args&&...)> func) const;

        Context Capture() const
        {
            return {*this};
        }

        bool Empty() const { return m_factories.empty() && m_instances.empty(); }

    private:
        Context(const Context& other)
        {
            for (auto& [type, factory] : other.m_factories)
            {
                if (factory->Scope == Scope::Shared)
                    m_factories[type] = std::shared_ptr(factory);
            }

            for (auto& [type, instance] : other.m_instances)
            {
                if (instance->Scope == Scope::Shared)
                    m_instances[type] = std::shared_ptr(instance);
            }
        }

        struct Scoppable
        {
            explicit Scoppable(const Scope scope) : Scoppable::Scope(scope) {};
            virtual ~Scoppable() = default;

            Context::Scope Scope;
        };

        using ScoppableMap = std::unordered_map<std::type_index, std::shared_ptr<Scoppable>>;

        template<typename T>
        struct Instance final : Scoppable
        {
            explicit Instance(std::unique_ptr<T> handle, const Context::Scope scope) :
                Scoppable(scope), Handle(std::move(handle)) {};

            std::unique_ptr<T> Handle;
        };

        template<typename T>
        struct Factory final : Scoppable
        {
            Factory(Builder<T> builder, Context::Scope scope) :
                Scoppable(std::move(scope)), Create(std::move(builder)) {};

            Context::Builder<T> Create;
        };

        template <typename T>
        decltype(auto) BuildParameter() const;

        template <typename Tuple, std::size_t... Is>
        auto BuildParameters(std::index_sequence<Is...>) const;

        template <typename Tuple>
        auto BuildParameters() const;

        mutable ScoppableMap m_factories;
        mutable ScoppableMap m_instances;
    };
}

#include <Genode/System/Context.inl>
