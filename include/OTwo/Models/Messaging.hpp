#pragma once

#include <OTwo/Models/Character.hpp>
#include <SFML/System/String.hpp>

struct ChatMessage
{
    CharacterInfo Sender;
    sf::String Content;

    CharacterInfo Recipient;
    // MegaphoneInfo Megaphone;
};
