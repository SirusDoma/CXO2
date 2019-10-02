#ifndef OMC_ARCHIVE_HPP
#define OMC_ARCHIVE_HPP

#include <Genode/IO.hpp>
#include <SFML/System/FileInputStream.hpp>

#include <Nx/IO/Archives/O2FileEntry.hpp>

#include <unordered_map>

struct OmcWaveHeader
{
    char SampleName[32];
    short AudioFormat;
    short ChannelCount;
    int SampleRate;
    int BitRate;
    short BlockAlign;
    short BitsPerSample;
    int UnkData;
    int ChunkSize;
};

struct OmcOggHeader
{
    char SampleName[32];
    int SampleSize;
};

class OmcArchive : public Gx::Archive
{
public:
    OmcArchive();
    ~OmcArchive();

    virtual bool Open(const std::string& fileName);
    virtual bool Contains(const std::string& name) const;

    Gx::Int64 GetFile(unsigned int index, Gx::Uint8** data) const;
    virtual Gx::Int64 GetFile(const std::string& name, Gx::Uint8** data) const;

    virtual std::vector<FileEntry> GetFileEntries() const;
    std::string GetExtension(const std::string& name) const;

private:
    static unsigned char* DecodeWave(unsigned char* in, int length);

    Gx::Uint64 Read(void* data, Gx::Uint64 size) const;
    virtual Gx::Int64 GetFile(const FileEntry* entry, Gx::Uint8** data) const;

    Gx::Uint16 m_wavCount, m_oggCount;
    Gx::Uint32 m_wavOffset, m_oggOffset;

    mutable std::unordered_map<unsigned int, O2FileEntry> m_entries;
    mutable sf::FileInputStream m_fileStream;
};

#endif
