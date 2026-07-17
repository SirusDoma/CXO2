#pragma once

#include <memory>
#include <string>

namespace Cx
{
    class NamedMutexImpl;

    class NamedMutex final
    {
    public:
        explicit NamedMutex(const std::string& name);
        NamedMutex(const NamedMutex& copy) = delete;
        NamedMutex(NamedMutex&& other) noexcept = delete;

        ~NamedMutex();

        bool IsLocked() const;

    private:
        std::unique_ptr<NamedMutexImpl> m_impl;
    };
}
