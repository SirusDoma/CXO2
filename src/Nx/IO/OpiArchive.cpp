#include <Nx/IO/OpiArchive.hpp>

OpiArchive::OpiArchive() :
    m_fileStream(),
    m_headers()
{
}

OpiArchive::~OpiArchive()
{
}

bool OpiArchive::Open(const std::string& fileName)
{
    if (!Archive::Open(fileName))
        return false;

    m_fileStream.open(fileName);
}

bool OpiArchive::Contains(const std::string& name)
{
    auto iterator = m_headers.find(name);
    return iterator != m_headers.end();
}

Gx::Int64 OpiArchive::GetFile(const std::string& name, Gx::Uint8** data) const
{
    return Gx::Int64();
}
