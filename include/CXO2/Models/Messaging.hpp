#pragma once

#include <CXO2/Models/Character.hpp>
#include <SFML/System/String.hpp>

namespace Cx
{
    struct ChatMessage
    {
        CharacterInfo Sender;
        sf::String Content;

        CharacterInfo Recipient;
        // MegaphoneInfo Megaphone;
    };
}
