#ifndef OMC_ARCHIVE_HPP
#define OMC_ARCHIVE_HPP

#include <SFML/System/FileInputStream.hpp>

#include <Genode/IO.hpp>
#include <Genode/System/Primitives.hpp>

#include <O2/Archives/O2FileEntry.hpp>

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

class OmcArchive : public Gx::Archive
{
public:
    OmcArchive();
    ~OmcArchive();

    virtual bool Open(const std::string& fileName);
    virtual bool Contains(const std::string& name) const;

    Gx::Int64 GetFile(unsigned int reference, Gx::Uint8** data) const;
    virtual Gx::Int64 GetFile(const std::string& name, Gx::Uint8** data) const;

    virtual std::vector<FileEntry> GetFileEntries() const;
    std::string GetExtension(const std::string& name) const;

private:
    static Gx::Uint8* DecodeWave(Gx::Uint8* in, int length, int *accKeyByte, int *accCounter);

    bool Read(void* data, Gx::Uint64 size) const;
    virtual Gx::Int64 GetFile(const FileEntry* entry, Gx::Uint8** data) const;

    OmcHeader m_header;
    mutable std::unordered_map<unsigned int, O2FileEntry> m_entries;
    mutable sf::FileInputStream m_fileStream;
};

#endif
