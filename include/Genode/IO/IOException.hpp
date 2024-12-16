#pragma once

#include <Genode/System/Exception.hpp>

#include <utility>

namespace Gx
{
    class IOException : public Exception
    {
    public:
        explicit IOException(const std::string& message) : Exception(message) {};
    };

    class ResourceLoadException final : public IOException
    {
    public:
        explicit ResourceLoadException(const std::string& message) : IOException(message) {};
        ResourceLoadException(std::string  id, const std::string& message) : IOException(message), m_resourceID(std::move(id)) {};

    private:
        std::string m_resourceID;
    };

    class ResourceStoreException final : public IOException
    {
    public:
        explicit ResourceStoreException(const std::string& id) : IOException("Failed to store specified resource id (" + id + ")"), m_resourceID(id) {};
        ResourceStoreException(const std::string& id, const std::string& message) : IOException(message), m_resourceID(id) {};

        [[nodiscard]] const std::string& GetResourceID() const { return m_resourceID; }

    private:
        std::string m_resourceID;
    };

    class ResourceAccessException final : public IOException
    {
    public:
        explicit ResourceAccessException(const std::string& id) : IOException("Resource with specified resource id (" + id + ") is not found"), m_resourceID(id) {};
        ResourceAccessException(const std::string& id, const std::string& message) : IOException(message), m_resourceID(id) {};

        [[nodiscard]] const std::string& GetResourceID() const { return m_resourceID; }

    private:
        std::string m_resourceID;
    };
}
