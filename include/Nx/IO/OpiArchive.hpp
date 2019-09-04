#ifndef OPI_ARCHIVE_HPP
#define OPI_ARCHIVE_HPP

#include <Genode/IO/Archive.hpp>
#include <SFML/System/FileInputStream.hpp>

#include <unordered_map>

class OpiArchive : public Gx::Archive
{
public:
    OpiArchive();
    ~OpiArchive();

    virtual bool Open(const std::string& fileName);
    virtual bool Contains(const std::string& name);
    virtual Gx::Int64 GetFile(const std::string& name, Gx::Uint8** data) const;

private:
    struct ItemHeader
    {
        std::string Identifier;
        Gx::Uint64  Offset;
        Gx::Uint64  Size;
    };

    sf::FileInputStream m_fileStream;
    std::unordered_map<std::string, ItemHeader> m_headers;
};

#endif