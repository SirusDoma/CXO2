#pragma once

#include <CXO2/Services/Service.hpp>
#include <CXO2/Network/MessageEnvelope.hpp>
#include <CXO2/Network/Responses/CharacterInfoResponse.hpp>
#include <CXO2/Network/Requests/EquipItemRequest.hpp>
#include <CXO2/Network/Responses/EquipItemResponse.hpp>

#include <cstdint>
#include <functional>

namespace Cx
{
    enum class EquipmentType : std::uint8_t;

    class CharacterService : public Service
    {
    public:
        virtual void GetCharacterInfo(const MessageCallback<CharacterInfoResponse>& callback) const = 0;

        virtual void Equip(
            const EquipItemRequest& request,
            const MessageCallback<EquipItemResponse>& callback
        ) const = 0;
    };
}
