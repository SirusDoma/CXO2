#ifndef O2_FILE_ENTRY_HPP
#define O2_FILE_ENTRY_HPP

#include <Genode/IO/Archive.hpp>

struct O2FileEntry : Gx::Archive::FileEntry
{
    Gx::Uint32 Offset = 0;
};

#endif
