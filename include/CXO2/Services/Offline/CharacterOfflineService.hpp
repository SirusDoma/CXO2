#pragma once

#include <CXO2/Services/CharacterService.hpp>

namespace Cx
{
    class SessionContext;
    class ItemFactory;
    class CharacterOfflineService : public CharacterService
    {
    public:
        CharacterOfflineService(SessionContext& session, ItemFactory& items);

        void GetCharacterInfo(const MessageCallback<CharacterInfoResponse>& callback) const override;

        void Equip(
            const EquipItemRequest& request,
            const MessageCallback<EquipItemResponse>& callback
        ) const override;

    private:
        SessionContext& m_session;
        ItemFactory&    m_items;
    };
}
