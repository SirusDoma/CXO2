namespace Gx
{
    template<class T>
    inline std::shared_ptr<T> ResourceContainer::Load(const std::string& name, Uint8* data, Int64 size)
    {
        auto resource = Get<T>(name);
        if (resource)
            return resource;

        if (size <= 0)
            return nullptr;
        else if (typeid(T) == typeid(sf::Texture))
            return std::reinterpret_pointer_cast<T>(LoadTexture(name, data, size));
        else if (typeid(T) == typeid(sf::Font))
            return std::reinterpret_pointer_cast<T>(LoadFont(name, data, size));

        return nullptr;
    }

    template<class T>
    inline std::shared_ptr<T> ResourceContainer::Cache(const std::string& name, Uint8* data, Int64 size)
    {
        auto cache = GetCache<T>(name);
        if (cache)
            return cache;

		if (size <= 0)
			return nullptr;
        else if (typeid(T) == typeid(sf::Texture))
            return std::reinterpret_pointer_cast<T>(CacheTexture(name, data, size));
        else if (typeid(T) == typeid(sf::Font))
            return std::reinterpret_pointer_cast<T>(CacheFont(name, data, size));

        return nullptr;
    }

    template<class T>
    inline std::shared_ptr<T> ResourceContainer::Get(const std::string& name) const
    {
        auto iterator = m_resources.find(name);
        if (iterator != m_resources.end())
            return std::get<std::shared_ptr<T>>(iterator->second);

        return nullptr;
    }

    template<class T>
    inline std::shared_ptr<T> ResourceContainer::GetCache(const std::string& name) const
    {
        auto iterator = m_caches.find(name);
        if (iterator != m_caches.end())
        {
            auto cache = std::get<std::weak_ptr<T>>(iterator->second);
            if (!cache.expired())
                return cache.lock();
        }

        return nullptr;
    }
}