#include <OTwo/Services/CharacterService.hpp>
#include <OTwo/Network/NetworkAdapter.hpp>
#include <OTwo/Messages/Requests/CharacterInfoRequest.hpp>
#include <OTwo/Messages/Responses/CharacterInfoResponse.hpp>
#include <OTwo/Messages/Responses/EquipItemResponse.hpp>

CharacterOnlineService::CharacterOnlineService(NetworkAdapter& adapter) :
    m_adapter(adapter)
{
}

void CharacterOnlineService::GetCharacterInfo(const std::function<void(const CharacterInfo&)> callback) const
{
    m_adapter.Exchange<CharacterInfoRequest, CharacterInfoResponse>
    (
        CharacterInfoRequest{},
        [callback] (const auto& response)
        {
            auto charInfo = CharacterInfo
            {
                response.Name,
                response.Gender,
                response.Role,
                response.Level,
                response.Experience,
                CharacterInfo::RankStatsInfo{
                    0,
                    response.Wins,
                    response.Loses,
                    response.Draws
                },
                CharacterInfo::WalletInfo{
                    response.Gem,
                    response.Point
                },
                response.EquippedItemIDs.GetContainer(),
                {},
                {}
            };

            for (const std::uint32_t id : response.Inventory.GetContainer())
                charInfo.Inventory.push_back(id);

            callback(charInfo);
        }
    );
}

void CharacterOnlineService::Equip(
    const EquipItemRequest& request,
    std::function<void(const EquipItemResponse&)> callback,
    const std::function<void(const NetworkException&)> errorCallback
) const
{
    m_adapter.Exchange<EquipItemRequest, EquipItemResponse>(
        request,
        [callback] (const auto& response)
        {
            if (callback)
                callback(response);
        },
        errorCallback
    );
}
