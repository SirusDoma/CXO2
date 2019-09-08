namespace Gx
{
    template<class T>
    inline std::shared_ptr<T> CacheManager::Add(const std::string& name, Uint8* data, Int64 size)
    {
        auto cache = Get<T>(name);
        if (cache)
            return cache;

		if (size <= 0)
			return nullptr;

		auto deserializer = Gx::ResourceLoaderFactory::GetLoader<T>();
		if (deserializer)
		{
			auto deleter = [=](T* cache) { 
				Remove(name);
				delete cache;
			};

			auto resource    = std::shared_ptr<T>(new T(deserializer->Deserialize(data, size)), deleter);
			m_cacheMap[name] = resource;

			return resource;
		}

        return nullptr;
    }

    template<class T>
    inline std::shared_ptr<T> CacheManager::Get(const std::string& name) const
    {
        auto iterator = m_cacheMap.find(name);
        if (iterator != m_cacheMap.end())
        {
            std::weak_ptr<T> cache = std::get<std::weak_ptr<T>>(iterator->second);
			
			// Thread-safe comparison
			if (cache.expired())
				return nullptr; 

			return cache.lock();
        }

        return nullptr;
    }
}