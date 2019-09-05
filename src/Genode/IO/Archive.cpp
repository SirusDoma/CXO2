#include <Genode/IO/Archive.hpp>
#include <Genode/IO/FileSystem.hpp>

namespace Gx
{
    Archive::Archive() :
        m_filename()
    {
    }

    Archive::~Archive()
    {
    }

	Int64 Archive::FileEntry::GetFile(Uint8** data) const
	{
		return Parent->GetFile(this, data);
	}

    bool Archive::Open(const std::string& fileName)
    {
        if (!FileSystem::Instance()->Exists(fileName))
            return false;

        m_filename = fileName;
        return true;
    }

    std::string Archive::GetFileName() const
    {
        return m_filename;
    }
}