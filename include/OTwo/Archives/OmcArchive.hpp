#ifndef O2JAM_OMC_ARCHIVE_HPP
#define O2JAM_OMC_ARCHIVE_HPP

#include <SFML/System/FileInputStream.hpp>

#include <Genode/IO.hpp>

#include <OTwo/Archives/FileInfo.hpp>

#include <unordered_map>

struct OmcHeader
{
    char  Signature[4]; // "OMC" or "OJM"
    short FxCount;
    short BgCount;
    int   FxStartOffset;
    int   BgStartOffset;
    int   FileSize;
};

struct OmcWaveHeader
{
    char  Name[32];
    short AudioFormat;
    short ChannelCount;
    int   SampleRate;
    int   BitRate;
    short BlockAlign;
    short BitsPerSample;
    int   UnkData;
    int   ChunkSize;
};

struct OmcOggHeader
{
    char Name[32];
    int  Size;
};

class OmcArchive : public virtual Gx::Archive
{
public:
    OmcArchive() = default;
    ~OmcArchive() override = default;

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
    std::string GetExtension(const std::string& name) const;

private:
    static std::uint8_t* DecodeWave(std::uint8_t* in, int length, int* accKeyByte, int* accCounter);

    bool ReadStream(void* data, std::uint64_t size) const;

    OmcHeader m_header = OmcHeader();
    mutable std::unordered_map<unsigned int, FileInfo> m_entries;
    mutable sf::FileInputStream m_fileStream;
};

#endif
