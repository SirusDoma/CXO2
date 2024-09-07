#ifndef GENODE_SYSTEM_PROVIDER_HPP
#define GENODE_SYSTEM_PROVIDER_HPP

#include <Genode/Utilities/Reflection.hpp>

#include <functional>
#include <typeindex>
#include <unordered_map>
#include <memory>
#include <type_traits>

namespace Gx
{
    class Provider
    {
    public:
        template<typename T>
        using Builder = std::function<std::unique_ptr<T>(Provider&)>;

        enum class Scope
        {
            Local,
            Singleton
        };

        Provider() = default;
        virtual ~Provider() = default;

        template<typename T>
        void Provide(Scope scope = Scope::Local);

        template<typename T>
        void Provide(Builder<T> builder, Scope scope = Scope::Singleton);

        template<typename T>
        Builder<T> As();

        template<typename T>
        std::enable_if_t<!std::is_pointer_v<T>, T&> Require();

        template<typename T>
        std::enable_if_t<std::is_pointer_v<T>, T> Require();

        Provider CreateScope() { return Provider(*this); }

    private:
        Provider(Provider& other) :
            m_parent(&other),
            m_instances(),
            m_factories()
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

            Provider::Scope Scope;
        };
        using StorageMap = std::unordered_map<std::type_index, std::unique_ptr<Scoppable>>;

        template<typename T>
        struct Instance final : Scoppable
        {
            explicit Instance(std::unique_ptr<T> handle, const Provider::Scope scope) : Scoppable(scope), Handle(std::move(handle)) {};
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
            Factory(Provider::Builder<T> builder, Provider::Scope scope) : Scoppable(std::move(scope)), Builder(std::move(builder)) {};
            std::unique_ptr<Scoppable> Clone() override
            {
                if (Scoppable::Scope == Scope::Singleton)
                    return nullptr;

                return std::make_unique<Factory>(Builder, Scope);
            }

            Provider::Builder<T> Builder;
        };

        template <typename T>
        decltype(auto) BuildParameter();

        template <typename Tuple, std::size_t... Is>
        auto BuildParameters(std::index_sequence<Is...>);

        template <typename Tuple>
        auto BuildParameters();

        Provider*  m_parent;
        StorageMap m_instances;
        StorageMap m_factories;
    };
}

#include <Genode/System/Provider.inl>
#endif