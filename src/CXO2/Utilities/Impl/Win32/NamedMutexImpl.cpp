#include <CXO2/Utilities/Impl/Win32/NamedMutexImpl.hpp>

#include <windows.h>

namespace Cx
{
    NamedMutexImpl::NamedMutexImpl(const std::string& name) :
        m_handle(NULL),
        m_locked(false)
    {
        m_handle = CreateMutex(
            NULL,
            TRUE,
            name.c_str()
        );

        if (m_handle == NULL)
            return;

        m_locked = GetLastError() != ERROR_ALREADY_EXISTS;
    }

    NamedMutexImpl::~NamedMutexImpl()
    {
        if (m_handle != NULL && m_locked)
        {
            ReleaseMutex(m_handle);
            CloseHandle(m_handle);
        }
    }

    bool NamedMutexImpl::IsLocked() const
    {
        return m_locked;
    }
}
