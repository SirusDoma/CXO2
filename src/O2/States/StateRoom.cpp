#include <O2/States/StateRoom.hpp>

#include <Genode/IO/ResourceManager.hpp>
#include <Genode/Tasks.hpp>
#include <Genode/Fx.hpp>
#include <Genode/UI.hpp>

void StateRoom::Initialize()
{
    State::Initialize();

    auto background = Create<Gx::Sprite>("Metadata/State/Room/Background.json");
    AddChild(background);

    auto btnCreateRoom = Create<Gx::Button>("Metadata/State/Room/Btn_CreateRoom.json");
    AddChild(btnCreateRoom);

    auto btnBack = Create<Gx::Button>("Metadata/State/Room/Btn_Back.json");
    AddChild(btnBack);

    m_bgm = Create<sf::Music>("Metadata/State/Room/Music.json", Gx::ResourceScope::Shared);
    Play(m_bgm);
}
