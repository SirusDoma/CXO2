#ifndef GENODE_ASSET_MANAGER_HPP
#define GENODE_ASSET_MANAGER_HPP

#include <Genode/IO/FileSystem.hpp>

#include <vector>
#include <string>

namespace Gx
{
	class AssetManager : public FileSystem<AssetManager>
	{
	public:
		AssetManager();
		virtual ~AssetManager();

		static AssetManager* Instance();

		std::vector<std::string> GetAssetPaths();
		void AddAssetPath(const std::string& path);

		std::string GetFileName(const std::string& fullPath) const;
		std::string GetFullName(const std::string& fileName) const;

		virtual bool Contains(const std::string& fileName) const;
		virtual Int64 GetFile(const std::string& filename, Uint8** data) const;
		virtual std::vector<FileEntry> GetFileEntries() const;

	private:
		std::vector<std::string> m_directories;
	};
}

#endif