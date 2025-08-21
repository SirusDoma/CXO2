#pragma once

#include <boost/interprocess/sync/named_mutex.hpp>
#include <memory>
#include <string>

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
    std::unique_ptr<boost::interprocess::named_mutex> m_mutex;

    bool m_locked;
};