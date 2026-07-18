#pragma once

#include <CXO2/Services/CharacterService.hpp>

namespace Cx
{
    class NetworkService;
    class CharacterOnlineService : public CharacterService
    {
    public:
        explicit CharacterOnlineService(NetworkService& network);

        void GetCharacterInfo(const MessageCallback<CharacterInfoResponse>& callback) const override;

        void Equip(
            const EquipItemRequest& request,
            const MessageCallback<EquipItemResponse>& callback
        ) const override;

    private:
        NetworkService& m_network;
    };
}
