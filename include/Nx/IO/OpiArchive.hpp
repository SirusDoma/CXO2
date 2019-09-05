#ifndef OPI_ARCHIVE_HPP
#define OPI_ARCHIVE_HPP

#include <Genode/IO/Archive.hpp>
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
    virtual bool Contains(const std::string& name);
    virtual Gx::Int64 GetFile(const std::string& name, Gx::Uint8** data) const;

private:
    const unsigned int ITEM_HEADER_SIZE = 152;
    struct ItemHeader
    {
        std::string Identifier;
        Gx::Uint32  Offset = 0;
        Gx::Uint32  Size   = 0;
    };

    bool Read(void* data, Gx::Uint64 size);

    sf::FileInputStream m_fileStream;
    std::unordered_map<std::string, ItemHeader> m_headers;

    Signature m_signature;
};

#endif