#pragma once

#include <CXO2/Messages/Commands.hpp>
#include <CXO2/Messages/CollectionEnvelope.hpp>

#include <CXO2/Messages/ScoreEntry.hpp>


namespace Cx
{
    struct GameCompletedEventData
    {
        static constexpr Cx::Command Command = EventCommand::GameCompleted;

        CollectionEnvelope<std::vector<ScoreEntry>, std::uint32_t> Entries{};
    };
}
