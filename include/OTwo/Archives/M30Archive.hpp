#ifndef O2JAM_M30_ARCHIVE_HPP
#define O2JAM_M30_ARCHIVE_HPP

#include <SFML/System/FileInputStream.hpp>

#include <Genode/IO.hpp>
#include <Genode/System/Primitives.hpp>

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
    Gx::ResourcePtr<sf::InputStream> Open(const std::string &fileName) const override;

    bool Contains(const std::string& name) const override;

    std::vector<std::unique_ptr<Gx::FileInfo>> GetFileEntries() const override;
    std::unique_ptr<Gx::FileInfo> GetFileInfo(const std::string &fileName) const override;

    Gx::Int64 ReadFile(unsigned int index, void *data, Gx::Int64 size) const;
    Gx::Int64 ReadFile(const std::string &fileName, void *data, Gx::Int64 size) const override;
    void WriteFile(const std::string &fileName, void *data, Gx::Int64 size) override { throw Gx::NotSupportedException(); }

    Gx::Int64 GetFileSize(const std::string &fileName) const override;

private:
    bool ReadStream(void* data, Gx::Uint64 size) const;
    Gx::Int64 ReadFile(const FileInfo &entry, void *data, Gx::Int64 size) const;

    static void DecodeSample(std::vector<Gx::Uint8> &data, int encodingCode);

    M30Header m_header = M30Header();
    mutable std::unordered_map<unsigned int, FileInfo> m_entries;
    mutable sf::FileInputStream m_fileStream;
};

#endif
