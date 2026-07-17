#include <CXO2/Utilities/Impl/Unix/NamedMutexImpl.hpp>

#include <cctype>

#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>

namespace Cx
{
    NamedMutexImpl::NamedMutexImpl(const std::string& name) :
        m_fd(-1),
        m_locked(false)
    {
        std::string sanitized = name;
        for (char& ch : sanitized)
        {
            if (!std::isalnum(static_cast<unsigned char>(ch)) && ch != '.' && ch != '-' && ch != '_')
            {
                ch = '_';
            }
        }

        const auto path = "/tmp/" + sanitized;
        m_fd = open(path.c_str(), O_CREAT | O_RDWR, 0666);
        if (m_fd == -1)
            return;

        m_locked = flock(m_fd, LOCK_EX | LOCK_NB) == 0;
    }

    NamedMutexImpl::~NamedMutexImpl()
    {
        if (m_fd != -1)
        {
            if (m_locked)
                flock(m_fd, LOCK_UN);

            close(m_fd);
        }
    }

    bool NamedMutexImpl::IsLocked() const
    {
        return m_locked;
    }
}
