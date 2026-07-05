#include <CXO2/Services/CharacterService.hpp>
#include <CXO2/Services/MessageService.hpp>

#include <CXO2/Messages/Requests/CharacterInfoRequest.hpp>
#include <CXO2/Messages/Responses/CharacterInfoResponse.hpp>
#include <CXO2/Messages/Responses/EquipItemResponse.hpp>

namespace Cx
{
    CharacterOnlineService::CharacterOnlineService(MessageService& messages) :
        m_messages(messages)
    {
    }

    void CharacterOnlineService::GetCharacterInfo(const MessageCallback<CharacterInfoResponse>& callback) const
    {
        m_messages.Dispatch<CharacterInfoRequest, CharacterInfoResponse>
        (
            CharacterInfoRequest{},
            callback
        );
    }

    void CharacterOnlineService::Equip(
        const EquipItemRequest& request,
        const MessageCallback<EquipItemResponse>& callback
    ) const
    {
        m_messages.Dispatch<EquipItemRequest, EquipItemResponse>(request, callback);
    }
}
