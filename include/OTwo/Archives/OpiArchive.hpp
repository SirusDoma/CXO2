#ifndef O2JAM_OPI_ARCHIVE_HPP
#define O2JAM_OPI_ARCHIVE_HPP

#include <Genode/IO.hpp>
#include <SFML/System/FileInputStream.hpp>

#include <OTwo/Archives/FileInfo.hpp>

#include <unordered_map>

class OpiArchive final : public Gx::Archive
{
public:
    enum Signature : Gx::Uint32
    {
        Unknown,
        OPA = 01,
        OPI = 02,
    };

    OpiArchive() = default;
    ~OpiArchive() override = default;

    Signature GetSignature() const;
    bool LoadFromFile(const std::string& fileName) override;

    Gx::ResourcePtr<sf::InputStream> Open(const std::string &fileName) const override;

    bool Contains(const std::string& name) const override;
    std::vector<Gx::FileInfo> GetFileEntries() const override;
    std::unique_ptr<Gx::FileInfo> GetFileInfo(const std::string &fileName) const override;

    Gx::Int64 ReadFile(const std::string &fileName, void *data, Gx::Int64 size) const override;
    void WriteFile(const std::string &fileName, void *data, Gx::Int64 size) const override { throw Gx::NotSupportedException(); }

    Gx::Int64 GetFileSize(const std::string &fileName) const override;

private:
    const unsigned int ITEM_HEADER_SIZE = 152;

    bool ReadStream(void* data, Gx::Uint64 size) const;
 
    Signature m_signature;
    Gx::Uint32 m_count;

    mutable std::unordered_map<std::string, FileInfo> m_entries;
    mutable sf::FileInputStream m_fileStream;
};

#endif