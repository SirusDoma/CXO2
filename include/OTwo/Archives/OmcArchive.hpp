#ifndef O2JAM_OMC_ARCHIVE_HPP
#define O2JAM_OMC_ARCHIVE_HPP

#include <SFML/System/FileInputStream.hpp>

#include <Genode/IO.hpp>
#include <Genode/System/Primitives.hpp>

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

class OmcArchive final : public Gx::Archive
{
public:
    OmcArchive() = default;
    ~OmcArchive() override = default;

    bool LoadFromFile(const std::string& fileName) override;

    Gx::ResourcePtr<sf::InputStream> Open(unsigned int index) const;
    Gx::ResourcePtr<sf::InputStream> Open(const std::string &fileName) const override;

    bool Contains(const std::string& name) const override;
    std::vector<Gx::FileInfo> GetFileEntries() const override;
    std::unique_ptr<Gx::FileInfo> GetFileInfo(const std::string &fileName) const override;

    Gx::Int64 ReadFile(unsigned int index, void *data, Gx::Int64 size) const;
    Gx::Int64 ReadFile(const std::string &fileName, void *data, Gx::Int64 size) const override;
    void WriteFile(const std::string &fileName, void *data, Gx::Int64 size) const override { throw Gx::NotSupportedException(); }

    Gx::Int64 GetFileSize(const std::string &fileName) const override;
    std::string GetExtension(const std::string& name) const;

private:
    static Gx::Uint8* DecodeWave(Gx::Uint8* in, int length, int *accKeyByte, int *accCounter);

    bool ReadStream(void* data, Gx::Uint64 size) const;

    OmcHeader m_header;
    mutable std::unordered_map<unsigned int, FileInfo> m_entries;
    mutable sf::FileInputStream m_fileStream;
};

#endif
