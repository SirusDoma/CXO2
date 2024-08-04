#ifndef O2JAM_OJM_ARCHIVE_HPP
#define O2JAM_OJM_ARCHIVE_HPP

#include <OTwo/Archives/FileInfo.hpp>
#include <OTwo/Archives/M30Archive.hpp>
#include <OTwo/Archives/OmcArchive.hpp>

#include <Genode/IO/Archive.hpp>
#include <Genode/System/Exception.hpp>

#include <SFML/System/FileInputStream.hpp>

#include <unordered_map>

class OjmArchive final : public virtual Gx::Archive, M30Archive, OmcArchive
{
public:
    OjmArchive() = default;
    ~OjmArchive() override = default;

    bool LoadFromFile(const std::string& fileName) override;

    Gx::ResourcePtr<sf::InputStream> Open(unsigned int index) const override;
    Gx::ResourcePtr<sf::InputStream> Open(const std::string &fileName) const override;

    bool Contains(const std::string& name) const override;
    std::vector<std::unique_ptr<Gx::FileInfo>> GetFileEntries() const override;
    std::unique_ptr<Gx::FileInfo> GetFileInfo(const std::string &fileName) const override;

    Gx::Int64 ReadFile(const std::string &fileName, void *data, Gx::Int64 size) const override;
    void WriteFile(const std::string &fileName, void *data, Gx::Int64 size) override { throw Gx::NotSupportedException(); }

    Gx::Int64 GetFileSize(const std::string &fileName) const override;

private:
    enum class ArchiveType
    {
        Unknown,
        M30,
        OMC
    };

    ArchiveType m_type = ArchiveType::Unknown;
};

#endif
