#include <CXO2/Services/CharacterService.hpp>
#include <CXO2/Services/NetworkService.hpp>

#include <CXO2/Network/Requests/CharacterInfoRequest.hpp>
#include <CXO2/Network/Responses/CharacterInfoResponse.hpp>
#include <CXO2/Network/Responses/EquipItemResponse.hpp>

namespace Cx
{
    CharacterOnlineService::CharacterOnlineService(NetworkService& network) :
        m_network(network)
    {
    }

    void CharacterOnlineService::GetCharacterInfo(const MessageCallback<CharacterInfoResponse>& callback) const
    {
        m_network.Dispatch<CharacterInfoRequest, CharacterInfoResponse>
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
        m_network.Dispatch<EquipItemRequest, EquipItemResponse>(request, callback);
    }
}
