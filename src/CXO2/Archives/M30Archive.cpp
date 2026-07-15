#include <CXO2/Archives/M30Archive.hpp>
#include <Genode/IO/BufferedInputStream.hpp>
#include <Genode/Utilities/StringHelper.hpp>

namespace Cx
{
    bool M30Archive::LoadFromFile(const std::filesystem::path& fileName)
    {
        if (!Archive::LoadFromFile(Gx::LocalFileSystem::Instance().GetFullName(fileName)))
            return false;

        if (!m_fileStream.open(Gx::LocalFileSystem::Instance().GetFullName(fileName)))
            return false;

        if (!m_fileStream.seek(0).has_value())
            return false;

        if (!ReadStream(&m_header, sizeof(m_header)))
            return false;

        if (const auto signature = std::string(m_header.Signature, 3); signature != "M30")
            return false;

        const auto entries = GetFileEntries();
        return !entries.empty();
    }

    Gx::ResourcePtr<sf::InputStream> M30Archive::Open(const std::filesystem::path& fileName) const
    {
        const auto header = GetFileInfo(fileName);
        if (!header)
            throw Gx::ResourceAccessException(fileName.string(), "The specified name is not found for this archive");

        auto data = std::vector<std::byte>(header->GetSize());
        if (!ReadFile(dynamic_cast<FileInfo&>(*header), data.data(), data.size()).has_value())
            return nullptr;

        std::unique_ptr<sf::InputStream> stream = std::make_unique<Gx::BufferedInputStream>(std::move(data));
        return stream;
    }

    Gx::ResourcePtr<sf::InputStream> M30Archive::Open(const unsigned int index) const
    {
        const auto iterator = m_entries.find(index);
        if (iterator == m_entries.end())
            throw Gx::ResourceAccessException(std::to_string(index), "The specified index is out of bounds for this archive");

        const auto header = &iterator->second;
        auto data = std::vector<std::byte>(header->GetSize());

        if (!ReadFile(*header, data.data(), data.size()).has_value())
            return nullptr;

        std::unique_ptr<sf::InputStream> stream = std::make_unique<Gx::BufferedInputStream>(std::move(data));
        return stream;
    }

    std::unique_ptr<Gx::FileInfo> M30Archive::GetFileInfo(const std::filesystem::path& fileName) const
    {
        const auto name = fileName.string();
        if (Gx::StringHelper::StartsWith(name, Gx::StringHelper::Split(GetPrefix(), '/').front()) && name.find(':') != std::string::npos)
        {
            const auto tokens = Gx::StringHelper::Split(name, ':');
            if (tokens.size() != 2)
                return nullptr;

            if (const auto it = m_entries.find(std::stoi(tokens[1])); it != m_entries.end() && it->second.GetSize() > 0)
                return std::make_unique<FileInfo>(it->second);

            return nullptr;
        }

        for (auto const& [key, header] : m_entries)
        {
            if (header.GetName() == name)
                return std::make_unique<FileInfo>(header);
        }

        return nullptr;
    }

    std::optional<std::size_t> M30Archive::ReadFile(const unsigned int index, void* data, const std::size_t size) const
    {
        const auto iterator = m_entries.find(index);
        if (iterator == m_entries.end())
            throw Gx::ResourceAccessException(std::to_string(index), "The specified index is out of bounds for this archive");

        return ReadFile(iterator->second, data, size);
    }

    std::optional<std::size_t> M30Archive::ReadFile(const std::filesystem::path& fileName, void* data, const std::size_t size) const
    {
        const auto header = GetFileInfo(fileName);
        if (!header)
            throw Gx::ResourceAccessException(fileName.string(), "The specified name is not found for this archive");

        return ReadFile(dynamic_cast<FileInfo&>(*header), data, size);
    }

    bool M30Archive::Contains(const std::filesystem::path& name) const
    {
        return GetFileInfo(name) != nullptr;
    }

    std::optional<std::size_t> M30Archive::GetFileSize(const std::filesystem::path& fileName) const
    {
        const auto header = GetFileInfo(fileName);
        if (!header)
            throw Gx::ResourceAccessException(fileName.string(), "The specified name is not found for this archive");

        return header->GetSize();
    }

    std::vector<std::unique_ptr<Gx::FileInfo>> M30Archive::GetFileEntries() const
    {
        std::vector<std::unique_ptr<Gx::FileInfo>> result;

        m_entries.clear();
        if (!m_fileStream.seek(m_header.SampleOffset).has_value())
            return result;

        for (unsigned int i = 0; i < m_header.SampleCount; i++)
        {
            const auto offset = m_fileStream.tell();
            if (!offset.has_value())
                continue;

            auto sampleHeader = M30SampleHeader();
            if (!ReadStream(&sampleHeader, sizeof(sampleHeader)))
                continue;

            if (!m_fileStream.seek(m_fileStream.tell().value() + sampleHeader.Size).has_value())
                continue;

            unsigned int reference = sampleHeader.Reference + (sampleHeader.CodecCode == 0 ? 1000 : 0);
            auto entry = FileInfo(
                *this,
                Gx::StringHelper::Trim(std::string(sampleHeader.Name, sizeof(sampleHeader.Name))).toAnsiString(),
                sampleHeader.Size,
                reference,
                offset.value()
            );

            m_entries[reference] = entry;
            result.push_back(std::make_unique<FileInfo>(entry));
        }

        return result;
    }

    bool M30Archive::ReadStream(void* data, const std::uint64_t size) const
    {
        return m_fileStream.read(data, static_cast<std::int64_t>(size)) == size;
    }

    std::optional<std::size_t> M30Archive::ReadFile(const FileInfo& entry, void* data, std::size_t size) const
    {
        auto sampleHeader = M30SampleHeader();
        if (!m_fileStream.seek(static_cast<std::int64_t>(entry.GetOffset())).has_value())
            return std::nullopt;

        if (!ReadStream(&sampleHeader, sizeof(sampleHeader)))
            return std::nullopt;

        auto sampleData = std::vector<std::uint8_t>(sampleHeader.Size);
        const auto read = m_fileStream.read(&sampleData[0], sampleHeader.Size);

        if (!read.has_value())
            return read;

        if (size > read)
            size = read.value();

        DecodeSample(sampleData, m_header.EncodingCode);
        memcpy(data, &sampleData[0], size);

        return read;
    }

    void M30Archive::DecodeSample(std::vector<std::uint8_t> &data, const int encodingCode)
    {
        if (encodingCode == 16) // nami
        {
            for (unsigned int i = 0; i + 3 < data.size(); i += 4)
            {
                data[i + 0] = 'n' ^ data[i + 0];
                data[i + 1] = 'a' ^ data[i + 1];
                data[i + 2] = 'm' ^ data[i + 2];
                data[i + 3] = 'i' ^ data[i + 3];
            }
        }
    }
}
