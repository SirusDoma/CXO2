#pragma once

#include <CXO2/Metadata/Chart/O2JamChartMetadata.hpp>

#include <string>
#include <vector>

namespace Cx
{
    struct O2JamMusicList
    {
        std::vector<O2JamChartMetadata> Charts;
        std::string Source;
    };
}
