namespace Gx
{
	template<typename T>
	inline T* ResourceManager::AddArchive(const std::string& fileName)
	{
		static_assert(std::is_base_of<Archive, T>::value, "Parameter must be a Gx::Archive");

		auto name = FileHelper::GetFileName(fileName);
		if (auto archive = GetArchive<T>(name); archive != nullptr)
			return archive;

		m_archives[name] = new T();
		if (!m_archives[name]->Open(FileHelper::GetFullName(fileName)))
		{
			m_archives.erase(m_archives.find(name));
			return nullptr;
		}

		for (auto entry : m_archives[name]->GetFileEntries())
			m_entries[entry.Name] = entry;

		return m_archives[name];
	}

	template<typename T>
	inline T* ResourceManager::GetArchive(const std::string& filename) const
	{
		static_assert(std::is_base_of<Archive, T>::value, "Parameter must be a Gx::Archive");

		auto name     = FileHelper::GetFileName(filename);
		auto iterator = m_archives.find(name);
		if (iterator != m_archives.end())
			return iterator->second;

		return nullptr;
	}

	template<typename T>
	inline std::shared_ptr<T> ResourceManager::Resolve(const std::string name)
	{
		// Load texture from cache / entries
		std::shared_ptr<T> texture;
		if (!m_cache->Contains(name))
		{
			auto fileEntry = m_entries[name];
			auto loader = ResourceLoaderFactory::GetLoader<T>();

			return m_cache->Add(loader->Load(fileEntry.GetContent(), fileEntry.Size));
		}

		return m_cache->Get<T>(name);
	}
	
	template<typename T>
	inline T* ResourceManager::Create(const std::string& name, bool cache)
	{
		// Definition of target resource
		auto definition = Gx::ResourceDefinition();

		// Find capable loader
		auto loader = ResourceLoaderFactory::GetDefinitionLoader<T>();
		if (!loader)
			return nullptr;

		// Create context for resource dependencies
		auto context = ResourceContext();

		// Load definition from entries
		if (!m_cache->Contains(name))
		{
			auto iterator = m_entries.find(name);
			if (iterator == m_entries.end())
				return nullptr; // No definition found with specified name

			Uint8* data;
			auto entry = iterator->second;
			auto size  = entry->GetContent(&data);
			if (size <= 0)
				return nullptr; // Failed to read definition

			definition = loader->Load(data, size);
		}
		else
			definition = *m_cache->Get<ResourceDefinition>(name);

		// Load texture from definition
		if (definition.Texture)
		{
			// Load texture from cache / entries
			context.Texture = Resolve<sf::Texture>(definition.Texture);
			if (!context.Texture)
				return nullptr; // Failed to load required texture
		}

		// Load font from definition
		if (definition.Font)
		{
			// Load font from cache / entries
			context.Font = Resolve<sf::Font>(definition.Font);
			if (!context.Font)
				return nullptr;  // Failed to load required font
		}

		return loader->Create(definition, context);
	}
	
	
}