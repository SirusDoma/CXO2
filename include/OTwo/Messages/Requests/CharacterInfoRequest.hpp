#pragma once

#include <OTwo/Messages/Commands.hpp>

struct CharacterInfoRequest
{
    static constexpr Command Command = RequestCommand::GetCharacterInfo;
};
