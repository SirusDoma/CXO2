#ifndef O2JAM_OPI_ARCHIVE_HPP
#define O2JAM_OPI_ARCHIVE_HPP

#include <Genode/IO.hpp>
#include <SFML/System/FileInputStream.hpp>

#include <O2/Archives/O2FileEntry.hpp>

#include <unordered_map>

class OpiArchive : public Gx::Archive
{
public:
    enum Signature : Gx::Uint32
    {
        Unknown,
        OPA = 01,
        OPI = 02,
    };

    OpiArchive();
    ~OpiArchive();

    Signature GetSignature() const;

    virtual bool Open(const std::string& fileName);
    virtual bool Contains(const std::string& name) const;

    virtual Gx::Int64 GetFile(const std::string& name, Gx::Uint8** data) const;
    virtual std::vector<FileEntry> GetFileEntries();

private:
    const unsigned int ITEM_HEADER_SIZE = 152;

    bool Read(void* data, Gx::Uint64 size) const;
    virtual Gx::Int64 GetFile(const Archive::FileEntry* entry, Gx::Uint8** data) const;
 
    Signature m_signature;
    Gx::Uint32 m_count;

    mutable std::unordered_map<std::string, O2FileEntry> m_entries;
    mutable sf::FileInputStream m_fileStream;
};

#endif