#pragma once

#include <string>

namespace Cx
{
    class NamedMutexImpl final
    {
    public:
        explicit NamedMutexImpl(const std::string& name);
        ~NamedMutexImpl();

        bool IsLocked() const;

    private:
        void* m_handle;
        bool m_locked;
    };
}
