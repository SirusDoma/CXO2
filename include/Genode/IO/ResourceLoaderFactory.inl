#include <Genode/IO/Loaders/TextureLoader.hpp>
#include <Genode/IO/Loaders/FontLoader.hpp>

namespace
{
    void EnsureDefaultDeserializersRegistered()
    {
        static bool registered = false;
        if (!registered)
        {
            Gx::ResourceLoaderFactory::Register<sf::Texture>(new Gx::priv::TextureLoader());
            Gx::ResourceLoaderFactory::Register<sf::Font>(new Gx::priv::FontLoader());

            registered = true;
        }
    }
}

namespace Gx
{
    template<typename T>
    inline static void ResourceLoaderFactory::Register(ResourceLoader<T>* deserializer)
    {
        Remove<T>();

        std::type_index type = typeid(T);
        m_loaders[type] = deserializer;
    }
    
    template<typename T>
    inline bool ResourceLoaderFactory::Remove()
    {
        std::type_index type = typeid(T);
        auto iterator = m_loaders.find(type);
        if (iterator != m_loaders.end())
        {
            delete iterator->second;
            m_loaders.erase(iterator);

            return true;
        }

        return false;
    }
    
    template<typename T>
    inline ResourceLoader<T>* ResourceLoaderFactory::GetLoader()
    {
        EnsureDefaultDeserializersRegistered();

        std::type_index type = typeid(T);
        auto iterator = m_loaders.find(type);
        if (iterator != m_loaders.end())
            return dynamic_cast<ResourceLoader<T>*>(iterator->second);

        return nullptr;
    }

    template<typename T>
    inline DefinitionLoader<T>* ResourceLoaderFactory::GetDefinitionLoader()
    {
        EnsureDefaultDeserializersRegistered();

        std::type_index type = typeid(T);
        auto iterator = m_loaders.find(type);
        if (iterator != m_loaders.end())
            return dynamic_cast<DefinitionLoader<T>*>(iterator->second);

        return nullptr;
    }
}