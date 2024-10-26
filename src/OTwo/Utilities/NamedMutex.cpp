#include <OTwo/Utilities/NamedMutex.hpp>

#if _WIN32
#include <windows.h>
#endif

NamedMutex::NamedMutex(const std::string& name):
    m_name(std::move(name)),
    m_handle(NULL),
    m_mutex(nullptr)
{
#if _WIN32
    m_handle = CreateMutex(
        NULL,
        TRUE,
        m_name.c_str()
    );

    if (m_handle == NULL)
    {
        m_locked = false;
        return;
    }

    m_locked = GetLastError() != ERROR_ALREADY_EXISTS;
#else
    try
    {
        m_mutex  = std::make_unique<boost::interprocess::named_mutex>(boost::interprocess::open_or_create, m_name.c_str());
        m_locked = m_mutex->try_lock();
        return;
    }
    catch (boost::interprocess::interprocess_exception)
    {
        m_mutex  = nullptr;
        m_locked = false;

        return;
    }
#endif
}

NamedMutex::~NamedMutex()
{
#if _WIN32
    if (m_handle != NULL && m_locked)
    {
        ReleaseMutex(m_handle);
        CloseHandle(m_handle);
    }
#else
    if (m_mutex && m_locked)
    {
        m_mutex->unlock();
        m_mutex->remove(m_name.c_str());
    }
#endif
}

bool NamedMutex::IsLocked() const
{
    return m_locked;
}
