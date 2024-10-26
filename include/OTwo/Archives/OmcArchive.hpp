#pragma once

#include <SFML/System/FileInputStream.hpp>

#include <Genode/IO.hpp>

#include <OTwo/Archives/FileInfo.hpp>

#include <unordered_map>

struct OmcHeader
{
    char Signature[4]; // "OMC" or "OJM"
    std::uint16_t FxCount;
    std::uint16_t BgCount;
    std::uint32_t FxStartOffset;
    std::uint32_t BgStartOffset;
    std::uint32_t FileSize;
};

struct OmcWaveHeader
{
    char Name[32];
    std::uint16_t AudioFormat;
    std::uint16_t ChannelCount;
    std::uint32_t SampleRate;
    std::uint32_t BitRate;
    std::uint16_t BlockAlign;
    std::uint16_t BitsPerSample;
    std::uint32_t UnkData;
    std::uint32_t ChunkSize;
};

struct OmcOggHeader
{
    char Name[32];
    std::uint32_t Size;
};

class OmcArchive : public virtual Gx::Archive
{
public:
    OmcArchive() = default;

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
    std::string GetExtension(const std::string& name) const;

private:
    static std::uint8_t* DecodeWave(std::uint8_t* in, int length, int* accKeyByte, int* accCounter);

    bool ReadStream(void* data, std::uint64_t size) const;

    OmcHeader m_header = OmcHeader();
    mutable std::unordered_map<unsigned int, FileInfo> m_entries;
    mutable sf::FileInputStream m_fileStream;
};
