#ifndef O2JAM_M30_ARCHIVE_HPP
#define O2JAM_M30_ARCHIVE_HPP

#include <SFML/System/FileInputStream.hpp>

#include <Genode/IO.hpp>
#include <Genode/System/Primitives.hpp>

#include <O2/Archives/O2FileEntry.hpp>

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

class M30Archive : public Gx::Archive
{
public:
    M30Archive();
    ~M30Archive();

    virtual bool Open(const std::string& fileName);
    virtual bool Contains(const std::string& name) const;

    Gx::Int64 GetFile(unsigned int index, Gx::Uint8** data) const;
    virtual Gx::Int64 GetFile(const std::string& name, Gx::Uint8** data) const;
    virtual std::vector<FileEntry> GetFileEntries() const;

private:
    static Gx::Uint8* DecodeSample(Gx::Uint8* in, int length, int encodingCode);

    bool Read(void* data, Gx::Uint64 size) const;
    virtual Gx::Int64 GetFile(const FileEntry* entry, Gx::Uint8** data) const;

    M30Header m_header;
    mutable std::unordered_map<unsigned int, O2FileEntry> m_entries;
    mutable sf::FileInputStream m_fileStream;
};

#endif
