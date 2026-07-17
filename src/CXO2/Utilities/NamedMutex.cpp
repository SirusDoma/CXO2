#include <CXO2/Utilities/NamedMutex.hpp>
#include <CXO2/Utilities/NamedMutexImpl.hpp>

namespace Cx
{
    NamedMutex::NamedMutex(const std::string& name) :
        m_impl(std::make_unique<NamedMutexImpl>(name))
    {
    }

    NamedMutex::~NamedMutex() = default;

    bool NamedMutex::IsLocked() const
    {
        return m_impl->IsLocked();
    }
}
