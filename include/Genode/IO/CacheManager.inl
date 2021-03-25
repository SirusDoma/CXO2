namespace Gx
{
    template<typename T>
    inline T* CacheManager::Add(const std::string& name, Uint8* data, Int64 size, bool useCache)
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

            return resource.get();
        }

        return nullptr;
    }

    template<typename T>
    inline T* CacheManager::Add(const std::string& name, const T& value, bool useCache)
    {
        if (useCache)
        {
            auto cache = Get<T>(name);
            if (cache)
                return cache;
        }

        auto resource    = std::make_shared<T>(value);
        m_cacheMap[name] = resource;

        return resource.get();
    }

    template<typename T>
    inline T* CacheManager::Get(const std::string& name) const
    {
        auto iterator = m_cacheMap.find(name);
        if (iterator != m_cacheMap.end())
            return std::static_pointer_cast<T>(iterator->second).get();

        return nullptr;
    }
}