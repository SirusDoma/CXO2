#pragma once

#include <CXO2/UI/Room/RoomButton.hpp>

#include <Genode/UI/List.hpp>

#include <vector>
#include <memory>
#include <map>

namespace sf
{
    class Sound;
}

namespace Gx
{
    class AudioMixer;
    class ResourceManager;
}

namespace Cx
{

    class RoomList : public Gx::List
    {
    public:
        RoomList(Gx::AudioMixer& mixer, Gx::ResourceManager& resources);

        void Initialize() override;

        void Upsert(const Room& room);
        Room& GetRoom(std::uint32_t id);
        void Remove(std::uint32_t id);
        void Clear();

        const Room* GetWaitingRoom();

        void ShowAll();
        void ShowWaitingOnly();

        void NextPage();
        void PreviousPage();

        void SetEnterRoomCallback(std::function<void(const Room&)> callback) const;

        void Invalidate() override;

    private:
        constexpr static unsigned int MAX_NUMBER_OF_ROOM = 100;

        void OnRoomButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnEnterRoomButtonClicked(Gx::Control& sender, Gx::Control::Event& ev) const;

        Gx::AudioMixer& m_mixer;
        Gx::ResourceManager& m_resources;

        std::map<unsigned int, Room> m_rooms;
        unsigned int m_page;
        bool m_waiting;

        mutable sf::Sound* m_sfxInvalid{nullptr};
        mutable std::function<void(const Room&)> m_enterRoomCallback;
    };
}
