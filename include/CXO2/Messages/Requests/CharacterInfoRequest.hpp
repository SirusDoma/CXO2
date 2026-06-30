#pragma once

#include <CXO2/Messages/Commands.hpp>

namespace Cx
{
    struct CharacterInfoRequest
    {
        static constexpr Cx::Command Command = RequestCommand::GetCharacterInfo;
    };
}
