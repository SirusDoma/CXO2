#pragma once

#include <CXO2/Config/GameConfig.hpp>

namespace Cx
{
    class GameConfigImpl
    {
    public:
        static void Load(GameConfig& config);
        static void Save(const GameConfig& config);
    };
}
