#ifndef OPI_ARCHIVE_HPP
#define OPI_ARCHIVE_HPP

#include <Genode/IO.hpp>
#include <SFML/System/FileInputStream.hpp>

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

private:
    const unsigned int ITEM_HEADER_SIZE = 152;
    struct OpiItemHeader : Gx::Archive::ItemHeader
    {
        Gx::Uint32  Offset = 0;
    };

    Gx::Uint64 Read(void* data, Gx::Uint64 size) const;
 
    Signature m_signature;
    std::unordered_map<std::string, OpiItemHeader> m_headers;

    mutable sf::FileInputStream m_fileStream;
};

#endif