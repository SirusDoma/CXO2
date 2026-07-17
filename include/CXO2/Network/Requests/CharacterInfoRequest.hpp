#pragma once

#include <CXO2/Network/Commands.hpp>

namespace Cx
{
    struct CharacterInfoRequest
    {
        static constexpr Cx::Command Command = RequestCommand::GetCharacterInfo;
    };
}
