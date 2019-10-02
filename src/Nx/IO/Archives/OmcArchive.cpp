#include <Nx/IO/Archives/OmcArchive.hpp>
#include <sstream>

OmcArchive::OmcArchive() :
    m_fileStream(),
    m_entries(),
    m_wavCount(),
    m_wavOffset(),
    m_oggCount(),
    m_oggOffset()
{
}

OmcArchive::~OmcArchive()
{
}

bool OmcArchive::Open(const std::string &fileName)
{
    if (!Archive::Open(fileName))
        return false;

    // Fetch meta data
    m_fileStream.open(fileName);
    m_fileStream.seek(0);

    char signbytes[4];
    if (!Read(&signbytes, sizeof(signbytes)))
        return false;

    auto signature = std::string(signbytes, 3);
    if (signature != "OMC" && signature != "OJM")
        return false;

    if (!Read(&m_wavCount, sizeof(m_wavCount)))
        return false;

    if (!Read(&m_oggCount, sizeof(m_oggCount)))
        return false;

    if (!Read(&m_wavOffset, sizeof(m_wavOffset)))
        return false;

    if (!Read(&m_oggOffset, sizeof(m_oggOffset)))
        return false;

    auto entries = GetFileEntries();
    return entries.size() > 0; // == m_wavCount + m_oggCount;
}

bool OmcArchive::Contains(const std::string &name) const
{
    for (auto const& [key, header] : m_entries)
    {
        if (header.Name == name)
            return true;
    }

    return false;
}

Gx::Int64 OmcArchive::GetFile(unsigned int index, Gx::Uint8 **data) const
{
    auto iterator = m_entries.find(index);
    if (iterator == m_entries.end())
        return -1;

    m_fileStream.seek(iterator->second.Offset);
    if (index < 1000)
    {
        OmcWaveHeader waveHeader;
        if (!Read(&waveHeader, sizeof(waveHeader)))
            return -1;

        auto encodedData = new unsigned char[waveHeader.ChunkSize];
        if (!Read(encodedData, waveHeader.ChunkSize))
            return -1;

        auto sampleData = DecodeWave(encodedData, waveHeader.ChunkSize);
        int pcm = 16, fileSize = waveHeader.ChunkSize + 36;

        // Build complete wave sample
        std::stringstream waveStream;
        waveStream.write("RIFF", 4);
        waveStream.write((char*)&fileSize, sizeof(fileSize));
        waveStream.write("WAVE", 4);
        waveStream.write("fmt ", 4);
        waveStream.write((char*)&pcm, sizeof(pcm));
        waveStream.write((char*)&waveHeader.AudioFormat  , sizeof(waveHeader.AudioFormat));
        waveStream.write((char*)&waveHeader.ChannelCount , sizeof(waveHeader.ChannelCount));
        waveStream.write((char*)&waveHeader.SampleRate   , sizeof(waveHeader.SampleRate));
        waveStream.write((char*)&waveHeader.BitRate      , sizeof(waveHeader.BitRate));
        waveStream.write((char*)&waveHeader.BlockAlign   , sizeof(waveHeader.BlockAlign));
        waveStream.write((char*)&waveHeader.BitsPerSample, sizeof(waveHeader.BitsPerSample));
        waveStream.write("data", 4);
        waveStream.write((char*)&waveHeader.ChunkSize    , sizeof(waveHeader.ChunkSize));
        waveStream.write((char*)sampleData, waveHeader.ChunkSize);

        auto buffer = waveStream.str();
        unsigned int read = buffer.length();

        *data = new unsigned char[read];
        memcpy((char*)&(*data)[0], buffer.data(), read);

        delete[] encodedData;
        delete[] sampleData;
        
        return read;
    }
    else
    {
        OmcOggHeader oggHeader;
        if (!Read(&oggHeader, sizeof(oggHeader)))
            return -1;

        *data = new unsigned char[oggHeader.SampleSize];
        return Read((char*)&(*data)[0], oggHeader.SampleSize);
    }
}

Gx::Int64 OmcArchive::GetFile(const std::string &name, Gx::Uint8 **data) const
{
    for (auto const& [key, header] : m_entries)
    {
        if (header.Name == name)
            return GetFile(key, data);
    }

    return -1;
}

Gx::Int64 OmcArchive::GetFile(const FileEntry* entry, Gx::Uint8** data) const
{
    for (auto const& [key, header] : m_entries)
    {
        if (header.Name == entry->Name)
            return GetFile(key, data);
    }

    return -1;
}

std::vector<Gx::Archive::FileEntry> OmcArchive::GetFileEntries() const
{
    std::vector<FileEntry> result;
    m_entries.clear();

    m_fileStream.seek(m_wavOffset);
    for (unsigned int i = 0; i < m_wavCount; i++)
    {
        auto entry   = O2FileEntry();
        auto offset  = m_fileStream.tell();
        if (offset == -1)
            continue;

        auto waveHeader = OmcWaveHeader();
        if (!Read(&waveHeader, sizeof(waveHeader)))
            continue;

        if (m_fileStream.seek(m_fileStream.tell() + waveHeader.ChunkSize) == -1)
            continue;

        entry.Name   = std::string(waveHeader.SampleName, sizeof(waveHeader.SampleName)).c_str();
        entry.Size   = waveHeader.ChunkSize;
        entry.Offset = offset;

        m_entries[i] = entry;
        result.push_back(entry);
    }

    m_fileStream.seek(m_oggOffset);
    for (unsigned int i = 0; i < m_oggCount; i++)
    {
        auto entry   = O2FileEntry();
        auto offset  = m_fileStream.tell();
        if (offset == -1)
            continue;

        auto oggHeader = OmcOggHeader();
        if (!Read(&oggHeader, sizeof(oggHeader)))
            continue;

        if (m_fileStream.seek(m_fileStream.tell() + oggHeader.SampleSize) == -1)
            continue;

        entry.Name   = std::string(oggHeader.SampleName, sizeof(oggHeader.SampleName)).c_str();
        entry.Size   = oggHeader.SampleSize;
        entry.Offset = offset;

        m_entries[i + 1000] = entry;
        result.push_back(entry);
    }

    return result;
}

std::string OmcArchive::GetExtension(const std::string& name) const
{
    for (auto const& [key, header] : m_entries)
    {
        if (header.Name == name)
            return key < 1000 ? ".wav" : ".ogg";
    }

    return "";
}

Gx::Uint64 OmcArchive::Read(void *data, Gx::Uint64 size) const
{
    auto read = m_fileStream.read(data, size);
    return read == size;
}

static const unsigned char WAVE_REARRANGE_TABLE[] = {
        0x10, 0x0E, 0x02, 0x09, 0x04, 0x00, 0x07, 0x01,
        0x06, 0x08, 0x0F, 0x0A, 0x05, 0x0C, 0x03, 0x0D,
        0x0B, 0x07, 0x02, 0x0A, 0x0B, 0x03, 0x05, 0x0D,
        0x08, 0x04, 0x00, 0x0C, 0x06, 0x0F, 0x0E, 0x10,
        0x01, 0x09, 0x0C, 0x0D, 0x03, 0x00, 0x06, 0x09,
        0x0A, 0x01, 0x07, 0x08, 0x10, 0x02, 0x0B, 0x0E,
        0x04, 0x0F, 0x05, 0x08, 0x03, 0x04, 0x0D, 0x06,
        0x05, 0x0B, 0x10, 0x02, 0x0C, 0x07, 0x09, 0x0A,
        0x0F, 0x0E, 0x00, 0x01, 0x0F, 0x02, 0x0C, 0x0D,
        0x00, 0x04, 0x01, 0x05, 0x07, 0x03, 0x09, 0x10,
        0x06, 0x0B, 0x0A, 0x08, 0x0E, 0x00, 0x04, 0x0B,
        0x10, 0x0F, 0x0D, 0x0C, 0x06, 0x05, 0x07, 0x01,
        0x02, 0x03, 0x08, 0x09, 0x0A, 0x0E, 0x03, 0x10,
        0x08, 0x07, 0x06, 0x09, 0x0E, 0x0D, 0x00, 0x0A,
        0x0B, 0x04, 0x05, 0x0C, 0x02, 0x01, 0x0F, 0x04,
        0x0E, 0x10, 0x0F, 0x05, 0x08, 0x07, 0x0B, 0x00,
        0x01, 0x06, 0x02, 0x0C, 0x09, 0x03, 0x0A, 0x0D,
        0x06, 0x0D, 0x0E, 0x07, 0x10, 0x0A, 0x0B, 0x00,
        0x01, 0x0C, 0x0F, 0x02, 0x03, 0x08, 0x09, 0x04,
        0x05, 0x0A, 0x0C, 0x00, 0x08, 0x09, 0x0D, 0x03,
        0x04, 0x05, 0x10, 0x0E, 0x0F, 0x01, 0x02, 0x0B,
        0x06, 0x07, 0x05, 0x06, 0x0C, 0x04, 0x0D, 0x0F,
        0x07, 0x0E, 0x08, 0x01, 0x09, 0x02, 0x10, 0x0A,
        0x0B, 0x00, 0x03, 0x0B, 0x0F, 0x04, 0x0E, 0x03,
        0x01, 0x00, 0x02, 0x0D, 0x0C, 0x06, 0x07, 0x05,
        0x10, 0x09, 0x08, 0x0A, 0x03, 0x02, 0x01, 0x00,
        0x04, 0x0C, 0x0D, 0x0B, 0x10, 0x05, 0x06, 0x0F,
        0x0E, 0x07, 0x09, 0x0A, 0x08, 0x09, 0x0A, 0x00,
        0x07, 0x08, 0x06, 0x10, 0x03, 0x04, 0x01, 0x02,
        0x05, 0x0B, 0x0E, 0x0F, 0x0D, 0x0C, 0x0A, 0x06,
        0x09, 0x0C, 0x0B, 0x10, 0x07, 0x08, 0x00, 0x0F,
        0x03, 0x01, 0x02, 0x05, 0x0D, 0x0E, 0x04, 0x0D,
        0x00, 0x01, 0x0E, 0x02, 0x03, 0x08, 0x0B, 0x07,
        0x0C, 0x09, 0x05, 0x0A, 0x0F, 0x04, 0x06, 0x10,
        0x01, 0x0E, 0x02, 0x03, 0x0D, 0x0B, 0x07, 0x00,
        0x08, 0x0C, 0x09, 0x06, 0x0F, 0x10, 0x05, 0x0A,
        0x04, 0x00
};

unsigned char* OmcArchive::DecodeWave(unsigned char* in, int length)
{
    auto *out = new unsigned char[length];
    int key = ((length % 17) << 4) + (length % 17);
    int blockSize = length / 17;

    memcpy(out, in, length);
    int inOffset, outOffset;
    for(int block = 0; block < 17; block++)
    {
        inOffset = blockSize * block;
        outOffset = blockSize * WAVE_REARRANGE_TABLE[key];

        memcpy(&out[outOffset], &in[inOffset], blockSize);
        key++;
    }

    int tmp = 0;
    unsigned char currentByte = 0;

    static int accKeyByte = 0xFF;
    static int accCounter = 0;

    for(int i = 0; i < length; i++)
    {
        tmp = out[i];
        currentByte = out[i];

        if(((accKeyByte << accCounter) & 0x80) != 0)
            currentByte = (unsigned char) ~currentByte;

        out[i] = currentByte;
        accCounter++;

        if(accCounter > 7)
        {
            accCounter = 0;
            accKeyByte = tmp;
        }
    }

    return out;
}