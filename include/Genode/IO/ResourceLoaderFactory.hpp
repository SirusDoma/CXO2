#pragma once

#include <Genode/IO/Resource.hpp>
#include <Genode/Utilities/Extensions.hpp>

#include <typeindex>
#include <typeinfo>
#include <functional>
#include <unordered_map>
#include <map>
#include <memory>
#include <string>

namespace Gx
{
    template<typename T>
    class ResourceLoader;
    class ResourceContext;
    class Context;

    class ResourceLoaderFactory final
    {
    public:
        ResourceLoaderFactory() = delete;
        ~ResourceLoaderFactory() = delete;

        static void BindContext(const Context& context);

        template<typename R, typename L>
        static void Register();

        template<typename R, typename L, typename U = std::string>
        static void Register(const type_identity_t<U>& id);

        template<typename R>
        static void Register(std::function<std::unique_ptr<ResourceLoader<R>>()> builder);

        template<typename B, typename R, typename L>
        static void Register();

        template<typename B, typename R>
        static void Register(std::function<std::unique_ptr<ResourceLoader<R>>()> builder);

        template<typename R, typename U = std::string>
        static void Register(const type_identity_t<U>& id, std::function<std::unique_ptr<ResourceLoader<R>>()> builder);

        template<typename B, typename R, typename L, typename U = std::string>
        static void Register(const type_identity_t<U>& id);

        template<typename B, typename R, typename U = std::string>
        static void Register(const type_identity_t<U>& id, std::function<std::unique_ptr<ResourceLoader<R>>()> builder);

        template<typename B, typename R>
        static void Reuse();

        template<typename B, typename R, typename U = std::string>
        static void Reuse(const type_identity_t<U>& id);

        template<typename B, typename R, typename ... Args>
        static void Reuse(const std::function<std::unique_ptr<R>(const ResourceContext&, Args...)>& instantiator);

        template<typename B, typename R, typename U = std::string, typename ... Args>
        static void Reuse(const type_identity_t<U>& id, const std::function<std::unique_ptr<R>(const ResourceContext&, Args...)>& instantiator);

        template<typename S, typename B, typename R>
        static void Reuse();

        template<typename S, typename B, typename R, typename U = std::string>
        static void Reuse(const type_identity_t<U>& id);

        template<typename S, typename B, typename R, typename ... Args>
        static void Reuse(const std::function<std::unique_ptr<R>(const ResourceContext&, Args...)>& instantiator);

        template<typename S, typename B, typename R, typename U = std::string, typename ... Args>
        static void Reuse(const type_identity_t<U>& id, const std::function<std::unique_ptr<R>(const ResourceContext&, Args...)>& instantiator);

        template<typename R>
        static bool Remove();

        template<typename R, typename U = std::string>
        static bool Remove(const type_identity_t<U>& id);

        template<typename R>
        static std::unique_ptr<ResourceLoader<R>> CreateLoader();

        template<typename R, typename U = std::string>
        static std::unique_ptr<ResourceLoader<R>> CreateLoader(const type_identity_t<U>& id);

    private:
        static void EnsureDefaultLoadersRegistered();

        struct BaseLoaderBuilder
        {
            virtual ~BaseLoaderBuilder() = default;
        };

        template<typename R>
        struct LoaderBuilder : BaseLoaderBuilder
        {
            LoaderBuilder() {}
            explicit LoaderBuilder(std::function<std::unique_ptr<ResourceLoader<R>>()> instantiator) : Instantiate(instantiator) {}

            std::function<std::unique_ptr<ResourceLoader<R>>()> Instantiate;
        };

        template<typename B, typename R>
        class AdaptorLoader;
        
        template<typename R, typename L>
        static std::unique_ptr<LoaderBuilder<R>> CreateLoaderBuilder();

        struct LoaderKey
        {
            template<typename T>
            explicit LoaderKey(const T& value) : Key(std::make_unique<KeyStorage<T>>(value)) {}

            bool operator<(const LoaderKey& other) const
            {
                if (Key->GetType() != other.Key->GetType())
                    return Key->GetType().before(other.Key->GetType());

                return Key->LessThan(*other.Key);
            }

            bool operator==(const LoaderKey& other) const
            {
                return Key->GetType() == other.Key->GetType() && Key->Equals(*other.Key);
            }

            struct BaseKeyStorage
            {
                virtual ~BaseKeyStorage() = default;
                virtual const std::type_info& GetType() const = 0;
                virtual bool LessThan(const BaseKeyStorage& other) const = 0;
                virtual bool Equals(const BaseKeyStorage& other) const = 0;
            };

            template<typename T>
            struct KeyStorage : BaseKeyStorage
            {
                T value;
                explicit KeyStorage(const T& v) : value(v) {}

                const std::type_info& GetType() const override { return typeid(T); }

                bool LessThan(const BaseKeyStorage& other) const override
                {
                    return value < static_cast<const KeyStorage&>(other).value;
                }

                bool Equals(const BaseKeyStorage& other) const override
                {
                    return value == static_cast<const KeyStorage&>(other).value;
                }
            };

            std::unique_ptr<BaseKeyStorage> Key;
        };
        
        using LoaderMap = std::unordered_map<std::type_index, std::map<LoaderKey, std::unique_ptr<BaseLoaderBuilder>>>;

        inline static const Context* m_context;
        inline static LoaderMap      m_loaders;
    };
}

#include <Genode/IO/ResourceLoaderFactory.inl>
