#ifndef O2JAM_OPI_ARCHIVE_HPP
#define O2JAM_OPI_ARCHIVE_HPP

#include <Genode/IO.hpp>
#include <SFML/System/FileInputStream.hpp>

#include <OTwo/Archives/FileInfo.hpp>

#include <unordered_map>

class OpiArchive final : public virtual Gx::Archive
{
public:
    enum class Signature : std::uint32_t
    {
        Unknown,
        OPA = 01,
        OPI = 02,
    };

    OpiArchive() = default;
    ~OpiArchive() override = default;

    Signature GetSignature() const;
    bool LoadFromFile(const std::string& fileName) override;

    Gx::ResourcePtr<sf::InputStream> Open(const std::string& fileName) const override;

    bool Contains(const std::string& name) const override;
    std::vector<std::unique_ptr<Gx::FileInfo>> GetFileEntries() const override;
    std::unique_ptr<Gx::FileInfo> GetFileInfo(const std::string& fileName) const override;

    std::int64_t ReadFile(const std::string& fileName, void* data, std::int64_t size) const override;
    void WriteFile(const std::string& fileName, void* data, std::int64_t size) override { throw Gx::NotSupportedException(); }

    std::int64_t GetFileSize(const std::string& fileName) const override;

private:
    const unsigned int ITEM_HEADER_SIZE = 152;

    bool ReadStream(void* data, std::uint64_t size) const;
 
    Signature m_signature;
    std::uint32_t m_count;

    mutable std::unordered_map<std::string, FileInfo> m_entries;
    mutable sf::FileInputStream m_fileStream;
};

#endif