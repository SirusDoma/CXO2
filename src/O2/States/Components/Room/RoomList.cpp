#include <O2/States/Components/Room/RoomList.hpp>

#include <Genode/UI/List.hpp>

RoomList::RoomList() :
    m_rooms(),
    m_roomButtons(),
    m_filterWaiting(false),
    m_page(1)
{
}

void RoomList::Initialize(Gx::Scene &scene)
{
    auto sfxAccept   = scene.Create<sf::Sound>("Metadata/State/Room/Sound/Accept.json", Gx::ResourceScope::Shared);
    auto sfxToggle   = scene.Create<sf::Sound>("Metadata/State/Room/Sound/Toggle.json", Gx::ResourceScope::Shared);
    auto sfxInvalid  = scene.Create<sf::Sound>("Metadata/State/Room/Sound/Invalid.json", Gx::ResourceScope::Shared);
    auto sfxNavigate = scene.Create<sf::Sound>("Metadata/State/Room/Sound/RoomNavigation.json", Gx::ResourceScope::Shared);

    auto roomList = scene.Create<Gx::List>("Metadata/State/Room/RoomList.json");
    for (unsigned int i = 0; i < ROOM_PER_PAGE; i++)
    {
        auto roomButton = std::make_unique<RoomButton>();
        roomButton->Initialize(scene);
        roomButton->SetClickCallback([=, button = roomButton.get()] (auto& sender, auto& ev)
        {
           if (!button->IsActive())
           {
               sfxInvalid->play();
               return;
           }
        });

        roomList->AddChild(roomButton.get());

        m_roomButtons.push_back(std::move(roomButton));
    }
    AddChild(roomList);

    auto btnCreateRoom = scene.Create<Gx::Button>("Metadata/State/Room/Btn_CreateRoom.json");
    btnCreateRoom->SetClickCallback([=] (auto& sender, auto& ev)
    {
        sfxAccept->play();
    });
    AddChild(btnCreateRoom);

    auto btnShortCut = scene.Create<Gx::Button>("Metadata/State/Room/Btn_ShortCut.json");
    AddChild(btnShortCut);

    auto btnShowAll     = scene.Create<Gx::Button>("Metadata/State/Room/Btn_ShowAll.json");
    auto btnWaitingRoom = scene.Create<Gx::Button>("Metadata/State/Room/Btn_WaitingRoom.json");
    btnShowAll->SetEnabled(false);
    btnShowAll->SetVisible(false);

    btnShowAll->SetClickCallback([=] (auto& sender, auto& ev)
    {
        sfxToggle->play();

        btnShowAll->SetEnabled(false);
        btnShowAll->SetVisible(false);
        btnWaitingRoom->SetEnabled(true);
        btnWaitingRoom->SetVisible(true);

        m_filterWaiting = false;
        m_page = 1;
        Invalidate();
    });

    btnWaitingRoom->SetClickCallback([=] (auto& sender, auto& ev)
    {
        sfxToggle->play();

        btnWaitingRoom->SetEnabled(false);
        btnWaitingRoom->SetVisible(false);
        btnShowAll->SetEnabled(true);
        btnShowAll->SetVisible(true);

        m_filterWaiting = true;
        m_page = 1;
        Invalidate();
    });
    AddChild(btnWaitingRoom, btnShowAll);

    auto btnRoomLeft  = scene.Create<Gx::Button>("Metadata/State/Room/Btn_RoomLeft.json");
    auto btnRoomRight = scene.Create<Gx::Button>("Metadata/State/Room/Btn_RoomRight.json");

    btnRoomLeft->SetClickCallback([=] (auto& sender, auto& ev)
    {
        sfxNavigate->play();

        if (m_page > 1)
        {
            m_page--;
            Invalidate();
        }
    });

    btnRoomRight->SetClickCallback([=] (auto& sender, auto& ev)
    {
        sfxNavigate->play();

        if (m_page < static_cast<unsigned int>(std::round(100.f / ROOM_PER_PAGE)))
        {
            m_page++;
            Invalidate();
        }
    });
    AddChild(btnRoomLeft, btnRoomRight);
}

void RoomList::PushRoomData(RoomData room)
{
    m_rooms[room.Number] = room;
    Invalidate();
}

void RoomList::Clear()
{
    m_rooms.clear();
    Invalidate();
}

void RoomList::Invalidate()
{
    m_page = std::min(m_page, static_cast<unsigned int>(std::round(100.f / ROOM_PER_PAGE)));
    m_page = std::max(m_page, static_cast<unsigned int>(1));

    for (size_t i = 0; i < m_roomButtons.size(); i++)
    {
        auto roomButton = m_roomButtons[i].get();
        auto roomNumber = ((m_page - 1) * ROOM_PER_PAGE) + i;
        auto room = m_rooms.find(roomNumber);
        if (room != m_rooms.end() && (!m_filterWaiting || room->second.State == RoomState::Waiting))
            roomButton->SetRoomData(room->second);
        else
            roomButton->Deactivate();
    }
}
