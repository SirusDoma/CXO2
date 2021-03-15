#include <O2/Scenes/StatePlanet.hpp>

#include <Genode/System/Application.hpp>
#include <Genode/IO/ResourceManager.hpp>
#include <Genode/Tasks.hpp>
#include <Genode/Fx.hpp>
#include <Genode/UI.hpp>

void StatePlanet::Initialize()
{
    auto background = Gx::ResourceManager::Instance()->Create<Gx::Sprite>("Metadata\\State_Planet\\Background.json");
    AddChild(background);

    auto tower = Gx::ResourceManager::Instance()->Create<Gx::Animation>("Metadata\\State_Planet\\Tower.json");
    AddChild(tower);

    auto exitButton = Gx::ResourceManager::Instance()->Create<Gx::Button>("Metadata\\State_Planet\\Btn_Exit.json");
    exitButton->SetClickCallback([=] { Gx::Application::Instance()->Close(); });
    AddChild(exitButton);

    auto philix   = Gx::ResourceManager::Instance()->Create<Gx::RadioButton>("Metadata\\State_Planet\\Btn_Philix.json");
    auto kleo     = Gx::ResourceManager::Instance()->Create<Gx::RadioButton>("Metadata\\State_Planet\\Btn_Kleo.json");
    auto kaliope  = Gx::ResourceManager::Instance()->Create<Gx::RadioButton>("Metadata\\State_Planet\\Btn_Kaliope.json");
    auto euta     = Gx::ResourceManager::Instance()->Create<Gx::RadioButton>("Metadata\\State_Planet\\Btn_Euta.json");
    auto thalo    = Gx::ResourceManager::Instance()->Create<Gx::RadioButton>("Metadata\\State_Planet\\Btn_Thalo.json");
    auto melpomin = Gx::ResourceManager::Instance()->Create<Gx::RadioButton>("Metadata\\State_Planet\\Btn_Melpomin.json");

    // TODO: UI Container
    philix->Pair(kleo, kaliope, euta, thalo, melpomin);
    std::unordered_map<Planet, Gx::RadioButton*> planets = {
        {Planet::Melpomin, melpomin},
        {Planet::Thalo,    thalo},
        {Planet::Euta,     euta},
        {Planet::Kaliope,  kaliope},
        {Planet::Kleo,     kleo},
        {Planet::Philix,   philix}
    };

    static auto sfx = sf::Sound();
    static auto sbf = sf::SoundBuffer();

    Gx::Uint8 *data;
    auto size = Gx::ResourceManager::Instance()->GetResourceData("click1", &data);
    if (sbf.loadFromMemory(data, size))
        sfx.setBuffer(sbf);

    for (auto [planet, radio] : planets)
    {
        radio->SetCheckStateChangeCallback([this, p = planet] {
            sfx.play();
            ShowChannelBoard(p);
        });
    }

    AddChild(philix, kleo, kaliope, euta, thalo, melpomin);

    // Overlay fade in animation
    auto overlay = new Gx::Rectangle(sf::Vector2f(800, 600));
    overlay->SetFillColor(sf::Color::White);
    AddChild(overlay);

    auto bgm = Gx::ResourceManager::Instance()->Create<sf::Music>("Metadata\\State_Planet\\Music.json");
    if (bgm)
        bgm->play();

    Run(new Gx::Sequence([=] { RemoveChild(overlay); }, {
        new Gx::Fade(overlay, 0, sf::seconds(2.5f))
    }));
}

void StatePlanet::ShowChannelBoard(Planet planet)
{
    static auto channelBoardPosition = sf::Vector2f();
    static auto sfx = sf::Sound();
    static auto sbf = sf::SoundBuffer();

    if (!m_channelBoard)
    {
        m_channelBoard = Gx::ResourceManager::Instance()->Create<Gx::Sprite>("Metadata\\State_Planet\\ChannelBoard.json");
        channelBoardPosition = m_channelBoard->GetPosition();

        Gx::Uint8 *data;
        auto size = Gx::ResourceManager::Instance()->GetResourceData("openChannel", &data);

        if (sbf.loadFromMemory(data, size))
            sfx.setBuffer(sbf);

        AddChild(m_channelBoard);
    }

    m_channelBoard->SetPosition(800 + m_channelBoard->GetLocalBounds().width, m_channelBoard->GetPosition().y);
    Run(new Gx::Sequence({
        new Gx::Action([=] { sfx.play(); }),
        new Gx::Move(m_channelBoard, channelBoardPosition - sf::Vector2f(30, 0), sf::milliseconds(250)),
        new Gx::Move(m_channelBoard, channelBoardPosition, sf::milliseconds(50))
    }));
}
