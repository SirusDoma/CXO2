#pragma once

#include <fstream>
#include <string>

namespace O2CX
{
    inline void Log(const std::string& message)
    {
        auto log = std::ofstream("o2cx.log", std::ios::app);
        log << message << std::endl;
    }
}
