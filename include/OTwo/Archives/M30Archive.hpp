#ifndef O2JAM_M30_ARCHIVE_HPP
#define O2JAM_M30_ARCHIVE_HPP

#include <SFML/System/FileInputStream.hpp>

#include <Genode/IO.hpp>

#include <OTwo/Archives/FileInfo.hpp>

#include <unordered_map>

struct M30Header
{
    char Signature[4];
    int  Version;
    int  EncodingCode;
    int  SampleCount;
    int  SampleOffset;
    int  Size;
    int  Padding;
};

struct M30SampleHeader
{
    char  Name[32];
    int   Size;
    short CodecCode;
    short UnkFixed;
    int   MusicFlag;
    short Reference;
    short UnkZero;
    int   PcmSamples;
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

    std::int64_t ReadFile(unsigned int index, void* data, std::int64_t size) const;

    std::int64_t ReadFile(const std::string& fileName, void* data, std::int64_t size) const override;
    void WriteFile(const std::string& fileName, void* data, std::int64_t size) override { throw Gx::NotSupportedException(); }

    std::int64_t GetFileSize(const std::string& fileName) const override;

private:
    bool ReadStream(void* data, std::uint64_t size) const;
    std::int64_t ReadFile(const FileInfo& entry, void* data, std::int64_t size) const;

    static void DecodeSample(std::vector<std::uint8_t> &data, int encodingCode);

    M30Header m_header = M30Header();
    mutable std::unordered_map<unsigned int, FileInfo> m_entries;
    mutable sf::FileInputStream m_fileStream;
};

#endif
