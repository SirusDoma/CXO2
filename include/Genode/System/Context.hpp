#ifndef GENODE_SYSTEM_CONTEXT_HPP
#define GENODE_SYSTEM_CONTEXT_HPP

#include <Genode/Utilities/Reflection.hpp>

#include <functional>
#include <typeindex>
#include <unordered_map>
#include <memory>
#include <type_traits>

namespace Gx
{
    class Context
    {
    public:
        template<typename T>
        using Builder = std::function<std::unique_ptr<T>(const Context&)>;

        enum class Scope
        {
            Local,
            Singleton
        };

        Context() = default;
        virtual ~Context() = default;

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

        Context CreateScope() const { return Context(*this); }

    private:
        Context(const Context& other) :
            m_factories(),
            m_instances()
        {
            for (auto& [type, factory] : other.m_factories)
            {
                if (auto clone = factory->Clone(); clone)
                    m_factories[type] = std::move(clone);
            }

            for (auto& [type, instance] : other.m_instances)
            {
                if (auto clone = instance->Clone(); clone)
                    m_instances[type] = std::move(clone);
            }
        }

        struct Scoppable
        {
            explicit Scoppable(const Scope scope) : Scoppable::Scope(scope) {};
            virtual ~Scoppable() = default;
            virtual std::unique_ptr<Scoppable> Clone() = 0;

            Context::Scope Scope;
        };
        using ScoppableMap = std::unordered_map<std::type_index, std::unique_ptr<Scoppable>>;

        template<typename T>
        struct Instance final : Scoppable
        {
            explicit Instance(std::unique_ptr<T> handle, const Context::Scope scope) : Scoppable(scope), Handle(std::move(handle)) {};
            std::unique_ptr<Scoppable> Clone() override
            {
                if (Scoppable::Scope == Scope::Local)
                    return nullptr;

                return std::make_unique<Instance>(std::unique_ptr<T>(Handle.get()), Scope);
            }

            std::unique_ptr<T> Handle;
        };

        template<typename T>
        struct Factory final : Scoppable
        {
            Factory(Builder<T> builder, Context::Scope scope) : Scoppable(std::move(scope)), Create(std::move(builder)) {};
            std::unique_ptr<Scoppable> Clone() override
            {
                if (Scoppable::Scope == Scope::Singleton)
                    return nullptr;

                return std::make_unique<Factory>(Create, Scope);
            }

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
#endif