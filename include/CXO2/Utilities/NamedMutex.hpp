#pragma once

#include <boost/interprocess/sync/file_lock.hpp>
#include <memory>
#include <string>

namespace Cx
{
    class NamedMutex final
    {
    public:
        explicit NamedMutex(const std::string& name);
        NamedMutex(const NamedMutex& copy) = delete;
        NamedMutex(NamedMutex&& other) noexcept = delete;

        ~NamedMutex();

        bool IsLocked() const;

    private:
        std::string m_name;

        void* m_handle;
        std::unique_ptr<boost::interprocess::file_lock> m_mutex;

        bool m_locked;
    };
}
