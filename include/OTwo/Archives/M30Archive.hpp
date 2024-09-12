#ifndef O2JAM_M30_ARCHIVE_HPP
#define O2JAM_M30_ARCHIVE_HPP

#include <SFML/System/FileInputStream.hpp>

#include <Genode/IO.hpp>

#include <OTwo/Archives/FileInfo.hpp>

#include <unordered_map>

struct M30Header
{
    char Signature[4];
    std::uint32_t Version;
    std::uint32_t EncodingCode;
    std::uint32_t SampleCount;
    std::uint32_t SampleOffset;
    std::uint32_t Size;
    std::uint32_t Padding;
};

struct M30SampleHeader
{
    char Name[32];
    std::uint32_t Size;
    std::uint16_t CodecCode;
    std::uint16_t UnkFixed;
    std::uint32_t MusicFlag;
    std::uint16_t Reference;
    std::uint16_t UnkZero;
    std::uint32_t PcmSamples;
};

class M30Archive : public virtual Gx::Archive
{
public:
    M30Archive() = default;
    ~M30Archive() override = default;

    bool LoadFromFile(const std::string& fileName) override;

    virtual Gx::ResourcePtr<sf::InputStream> Open(unsigned int index) const;
    Gx::ResourcePtr<sf::InputStream> Open(const std::string& fileName) const override;

    bool Contains(const std::string& name) const override;

    std::vector<std::unique_ptr<Gx::FileInfo>> GetFileEntries() const override;
    std::unique_ptr<Gx::FileInfo> GetFileInfo(const std::string& fileName) const override;

    std::optional<std::size_t> ReadFile(unsigned int index, void* data, std::size_t size) const;
    std::optional<std::size_t> ReadFile(const std::string& fileName, void* data, std::size_t size) const override;

    void WriteFile(const std::string& fileName, void* data, std::size_t size) override { throw Gx::NotSupportedException(); }

    std::optional<std::size_t> GetFileSize(const std::string& fileName) const override;

private:
    bool ReadStream(void* data, std::uint64_t size) const;
    std::optional<std::size_t> ReadFile(const FileInfo& entry, void* data, std::size_t size) const;

    static void DecodeSample(std::vector<std::uint8_t> &data, int encodingCode);

    M30Header m_header = M30Header();
    mutable std::unordered_map<unsigned int, FileInfo> m_entries;
    mutable sf::FileInputStream m_fileStream;
};

#endif
