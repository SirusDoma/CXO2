#pragma once

#if defined(_WIN32)
#include <CXO2/Utilities/Impl/Win32/NamedMutexImpl.hpp>
#else
#include <CXO2/Utilities/Impl/Unix/NamedMutexImpl.hpp>
#endif
