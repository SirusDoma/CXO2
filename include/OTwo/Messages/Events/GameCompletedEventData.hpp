#pragma once

#include <OTwo/Messages/Commands.hpp>
#include <OTwo/Messages/CollectionEnvelope.hpp>

#include <OTwo/Messages/ScoreEntry.hpp>


struct GameCompletedEventData
{
    static constexpr ::Command Command = EventCommand::GameCompleted;

    CollectionEnvelope<std::vector<ScoreEntry>, std::uint32_t> Entries{};
};
