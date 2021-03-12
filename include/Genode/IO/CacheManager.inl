namespace Gx
{
    template<typename T>
    inline std::shared_ptr<T> CacheManager::Add(const std::string& name, Uint8* data, Int64 size, bool useCache)
    {
        if (useCache)
        {
            auto cache = Get<T>(name);
            if (cache)
                return cache;
        }

        if (size <= 0)
            return nullptr;

        auto loader = Gx::ResourceLoaderFactory::GetLoader<T>();
        if (loader)
        {
            auto resource    = std::shared_ptr<T>(new T(loader->Load(data, size)));
            m_cacheMap[name] = resource;

            return resource;
        }

        return nullptr;
    }

    template<typename T>
    inline std::shared_ptr<T> CacheManager::Add(const std::string& name, T* value, bool useCache)
    {
        if (useCache)
        {
            auto cache = Get<T>(name);
            if (cache)
                return cache;
        }

        auto resource    = std::shared_ptr<T>(value);
        m_cacheMap[name] = resource;

        return resource;
    }

    template<typename T>
    inline std::shared_ptr<T> CacheManager::Get(const std::string& name) const
    {
        auto iterator = m_cacheMap.find(name);
        if (iterator != m_cacheMap.end())
            return std::get<std::shared_ptr<T>>(iterator->second);

        return nullptr;
    }
}