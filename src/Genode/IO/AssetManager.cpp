#include <Genode/IO/AssetManager.hpp>
#include <memory>
#include <algorithm>
#include <filesystem>

namespace Gx
{
	AssetManager::AssetManager() :
		m_directories()
	{
	}

	AssetManager::~AssetManager()
	{
		m_directories.clear();
	}

	AssetManager* AssetManager::Instance()
	{
		static AssetManager instance;
		return &instance;
	}

	std::vector<std::string> AssetManager::GetAssetPaths()
	{
		return m_directories;
	}

	void AssetManager::AddAssetPath(const std::string& path)
	{
		m_directories.push_back(path);
	}

	bool AssetManager::Contains(const std::string& fileName) const
	{
		auto path = std::filesystem::path(fileName.c_str());
		return std::filesystem::exists(path);
	}

	std::string AssetManager::GetFileName(const std::string& filename) const
	{
		auto path = std::filesystem::path(filename.c_str());
		return path.filename().string();
	}

	std::string AssetManager::GetFullName(const std::string& fileName) const
	{
		if (Contains(fileName))
			return fileName;

		for (auto path : m_directories)
		{
			std::string fullPath = path + "/" + fileName;
			if (Contains(fullPath))
				return fullPath;
		}

		return "";
	}

	Int64 AssetManager::GetFile(const std::string& filename, Uint8** data) const
	{
		sf::FileInputStream fs;
		fs.open(GetFullName(filename));

		auto size = fs.getSize();
		if (size <= 0)
			return size;

		*data = new Uint8[static_cast<unsigned int>(size)];
		return fs.read((char*) & (*data)[0], fs.getSize());
	}

	std::vector<AssetManager::FileEntry> AssetManager::GetFileEntries() const
	{
		// TODO: List all files within asset paths
		return std::vector<FileEntry>();
	}
}