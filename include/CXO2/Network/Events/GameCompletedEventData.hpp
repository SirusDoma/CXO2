#pragma once

#include <CXO2/Network/Commands.hpp>
#include <CXO2/Network/CollectionEnvelope.hpp>

#include <CXO2/Network/ScoreEntry.hpp>


namespace Cx
{
    struct GameCompletedEventData
    {
        static constexpr Cx::Command Command = EventCommand::GameCompleted;

        CollectionEnvelope<std::vector<ScoreEntry>, std::uint32_t> Entries{};
    };
}
