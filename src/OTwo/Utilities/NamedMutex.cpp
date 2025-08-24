#include <OTwo/Utilities/NamedMutex.hpp>

#if _WIN32
#include <windows.h>
#endif
#include <fstream>
#include <cctype>
#include <OTwo/Utilities/StringFormatter.hpp>

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
        std::string sanitized = m_name;
        for (char& ch : sanitized)
        {
            if (!std::isalnum(static_cast<unsigned char>(ch)) && ch != '.' && ch != '-' && ch != '_')
            {
                ch = '_';
            }
        }

        auto path = fmt::format("/tmp/{}", sanitized);
        {
            std::ofstream touchFile(path.c_str(), std::ios::app);
        }

        m_mutex  = std::make_unique<boost::interprocess::file_lock>(path.c_str());
        m_locked = m_mutex->try_lock();
        return;
    }
    catch (const boost::interprocess::interprocess_exception&)
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
    }
#endif
}

bool NamedMutex::IsLocked() const
{
    return m_locked;
}
