#include <Genode/IO/ResourceManager.hpp>
#include <Genode/IO/Cache.hpp>

namespace Gx
{
	ResourceManager::ResourceManager() :
		m_cache(Cache::Instance()),
		m_archives(),
		m_entries()
	{
	}
	
	ResourceManager::~ResourceManager()
	{
		for (auto entry : m_entries)
		{
			if (entry.second)
				delete entry.second;
		}

		for (auto archive : m_archives)
		{
			if (archive.second)
				delete archive.second;
		}

		if (m_cache)
			delete m_cache;
	}

	ResourceManager* ResourceManager::Instance()
	{
		static ResourceManager instance;
		return &instance;
	}

	Int64 ResourceManager::ReadResource(const std::string& path, Uint8** data)
	{
		std::string fullName = FileSystem::Instance()->GetFullName(path);
		if (FileSystem::Instance()->Exists(fullName))
			return FileSystem::Instance()->ReadFile(fullName, data);

		auto iterator = m_entries.find(path);
		if (iterator == m_entries.end())
			return -1;

		return m_entries[path]->GetContent(data);
	}
}