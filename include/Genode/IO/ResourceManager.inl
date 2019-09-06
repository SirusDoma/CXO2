namespace Gx
{
	template<class T>
	inline T* ResourceManager::AddArchive(const std::string& fileName)
	{
		static_assert(std::is_base_of<Archive, T>::value, "Parameter must be a Gx::Archive");

		auto name = AssetManager::Instance()->GetFileName(fileName);
		if (auto archive = GetArchive<T>(name); archive != nullptr)
			return archive;

		m_archives[name] = new T();
		if (!m_archives[name]->Open(FileSystem::Instance()->GetFullName(fileName)))
		{
			m_archives.erase(m_archives.find(name));
			return nullptr;
		}

		for (auto entry : m_archives[name]->GetFileEntries())
			m_entries[entry.Name] = entry;

		return m_archives[name];
	}

	template<class T>
	inline T* ResourceManager::GetArchive(const std::string& filename) const
	{
		static_assert(std::is_base_of<Archive, T>::value, "Parameter must be a Gx::Archive");

		auto name     = AssetManager::Instance()->GetFileName(filename);
		auto iterator = m_archives.find(name);
		if (iterator != m_archives.end())
			return iterator->second;

		return nullptr;
	}
	
	template<class T>
	inline T* ResourceManager::Instantiate(const std::string& name, bool cache)
	{

		return nullptr;
	}
}