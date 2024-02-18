#ifndef O2JAM_O2_FILE_ENTRY_HPP
#define O2JAM_O2_FILE_ENTRY_HPP

#include <Genode/IO/Archive.hpp>

struct O2FileEntry : Gx::Archive::FileEntry
{
    Gx::Uint64 Offset = 0;
};

#endif
