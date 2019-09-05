#include <Genode/IO/ResourceManager.hpp>

namespace Gx
{
	ResourceManager::ResourceManager()
	{
	}
	
	ResourceManager::~ResourceManager()
	{
	}

	ResourceManager* ResourceManager::Instance()
	{
		static ResourceManager instance;
		return &instance;
	}
}