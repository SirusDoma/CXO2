#pragma once

#if defined(_WIN32)
#include <CXO2/Utilities/Impl/Win32/StringTranscoderImpl.hpp>
#else
#include <CXO2/Utilities/Impl/Unix/StringTranscoderImpl.hpp>
#endif
