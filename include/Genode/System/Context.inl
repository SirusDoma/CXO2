#pragma once

#include <Genode/System/Exception.hpp>

#include <memory>
#include <functional>
#include <typeindex>
#include <type_traits>
#include <utility>
#include <string>

namespace Gx
{
    template <typename T, std::enable_if_t<Context::IsConstructible<T>, int>>
    void Context::Provide(Scope scope)
    {
        const auto key = std::type_index(typeid(T));
        auto svc       = std::make_shared<Service<T>>();
        svc->Lifetime  = scope;
        svc->Builder   = CreateBuilder<T>();
        m_entries[key] = std::move(svc);
    }

    template <typename TInterface, typename TConcrete,
        std::enable_if_t<
            std::is_base_of_v<TInterface, TConcrete> &&
            Context::IsConstructible<TConcrete>, int
        >
    >
    void Context::Provide(Scope scope)
    {
        const auto key = std::type_index(typeid(TInterface));
        auto svc       = std::make_shared<Service<TInterface>>();
        svc->Lifetime  = scope;
        svc->Builder   = [](const Context& c) -> std::unique_ptr<TInterface>
        {
            return CreateBuilder<TConcrete>()(c);
        };
        m_entries[key] = std::move(svc);
    }

    template <typename T>
    void Context::Provide(std::function<std::unique_ptr<T>(const Context&)> builder, Scope scope)
    {
        const auto key = std::type_index(typeid(T));
        auto svc       = std::make_shared<Service<T>>();
        svc->Lifetime  = scope;
        svc->Builder   = std::move(builder);
        m_entries[key] = std::move(svc);
    }

    template <typename T>
    std::enable_if_t<!std::is_pointer_v<T>, T&>
    Context::Require() const
    {
        using Type = std::remove_cv_t<std::remove_reference_t<T>>;
        const auto key = std::type_index(typeid(Type));

        if (auto* svc = GetService<Type>(key))
        {
            if (svc->Instance)
            {
                if (m_current)
                    m_current->Dependencies.push_back(m_entries[key]);

                return *svc->Instance;
            }

            auto* prev = m_current;
            m_current = svc;
            svc->Instance = svc->Builder(*this);
            m_current = prev;

            if (m_current)
                m_current->Dependencies.push_back(m_entries[key]);

            return *svc->Instance;
        }

        if (m_parent)
        {
            if (auto* ptr = m_parent->Require<Type*>())
                return *ptr;
        }

        if constexpr (IsConstructible<Type>)
        {
            ProvideDefault<Type>(key);
            return Require<T>();
        }
        else
        {
            throw InvalidOperationException(
                std::string("Gx::Context::Require — type not registered and cannot be resolved: ") +
                typeid(Type).name());
        }
    }

    template <typename T>
    std::enable_if_t<std::is_pointer_v<T>, T>
    Context::Require() const
    {
        using Type = std::remove_cv_t<std::remove_pointer_t<T>>;
        const auto key = std::type_index(typeid(Type));

        if (auto* svc = GetService<Type>(key))
        {
            if (svc->Instance)
            {
                if (m_current)
                    m_current->Dependencies.push_back(m_entries[key]);
                return svc->Instance.get();
            }

            auto* prev = m_current;
            m_current = svc;
            svc->Instance = svc->Builder(*this);
            m_current = prev;

            if (m_current)
                m_current->Dependencies.push_back(m_entries[key]);

            return svc->Instance.get();
        }

        if (m_parent)
            return m_parent->Require<T>();

        return nullptr;
    }

    template <typename T>
    std::unique_ptr<T> Context::Instantiate() const
    {
        using Type = std::remove_cv_t<std::remove_reference_t<T>>;
        const auto key = std::type_index(typeid(Type));

        if (auto* svc = GetService<Type>(key))
            return svc->Builder(*this);

        if (m_parent)
        {
            if (auto* ptr = m_parent->Require<Type*>())
                return m_parent->Instantiate<T>();
        }

        if constexpr (IsConstructible<Type>)
        {
            return CreateBuilder<Type>()(*this);
        }
        else
        {
            throw InvalidOperationException(
                std::string("Gx::Context::Instantiate — type not registered and cannot be resolved: ") +
                typeid(Type).name());
        }
    }

    template <typename T>
    std::shared_ptr<Context::Scopable> Context::Service<T>::Clone(bool reset) const
    {
        auto clone      = std::make_shared<Service<T>>();
        clone->Lifetime = Lifetime;
        clone->Builder  = Builder;
        clone->Instance = reset ? nullptr : Instance;
        return clone;
    }

    template <typename T, std::size_t... Is>
    std::unique_ptr<T> Context::Construct(const Context& c, std::index_sequence<Is...>)
    {
        return std::make_unique<T>(((void)Is, Resolver<T>{c})...);
    }

    template <typename T>
    std::function<std::unique_ptr<T>(const Context&)> Context::CreateBuilder()
    {
        static_assert(IsConstructible<T>,
            "Type is not constructible: no public constructor found within "
            "the arity limit, or the type is abstract. "
            "Use Provide<T>(builder) to register a factory manually.");

        return [](const Context& c) -> std::unique_ptr<T>
        {
            return Construct<T>(c, std::make_index_sequence<MinArityV<T>>{});
        };
    }

    template <typename T>
    Context::Service<T>* Context::GetService(std::type_index key) const
    {
        if (const auto it = m_entries.find(key); it != m_entries.end())
            return static_cast<Service<T>*>(it->second.get());

        return nullptr;
    }

    template <typename T>
    void Context::ProvideDefault(std::type_index key) const
    {
        auto svc       = std::make_shared<Service<T>>();
        svc->Lifetime  = Scope::Local;
        svc->Builder   = CreateBuilder<T>();
        m_entries[key] = std::move(svc);
    }

    template <typename Owner>
    template <typename T, typename>
    Context::Resolver<Owner>::operator T& () const
    {
        return Ctx.Require<T>();
    }

    template <typename Owner>
    template <typename T, typename>
    Context::Resolver<Owner>::operator T&& () const
    {
        return std::move(Ctx.Require<T>());
    }

    template <typename Owner>
    template <typename T, typename, typename>
    Context::Resolver<Owner>::operator T () const
    {
        return Ctx.Require<T>();
    }
}
