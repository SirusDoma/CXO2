#pragma once

#include <CXO2/Services/Service.hpp>
#include <CXO2/Messages/MessageEnvelope.hpp>
#include <CXO2/Messages/Responses/CharacterInfoResponse.hpp>
#include <CXO2/Messages/Requests/EquipItemRequest.hpp>
#include <CXO2/Messages/Responses/EquipItemResponse.hpp>

#include <cstdint>
#include <functional>

namespace Cx
{
    enum class EquipmentType : std::uint8_t;

    struct CharacterInfo;

    class CharacterService : public Service
    {
    public:
        virtual void GetCharacterInfo(const MessageCallback<CharacterInfoResponse>& callback) const = 0;

        virtual void Equip(
            const EquipItemRequest& request,
            const MessageCallback<EquipItemResponse>& callback
        ) const = 0;
    };

    class MessageService;
    class CharacterOnlineService : public CharacterService
    {
    public:
        explicit CharacterOnlineService(MessageService& messages);

        void GetCharacterInfo(const MessageCallback<CharacterInfoResponse>& callback) const override;

        void Equip(
            const EquipItemRequest& request,
            const MessageCallback<EquipItemResponse>& callback
        ) const override;

    private:
        MessageService& m_messages;
    };

    class CharacterOfflineService : CharacterService
    {
    public:
        void GetCharacterInfo(const MessageCallback<CharacterInfoResponse>& callback) const override;

        void Equip(
            const EquipItemRequest& request,
            const MessageCallback<EquipItemResponse>& callback
        ) const override;
    };
}
