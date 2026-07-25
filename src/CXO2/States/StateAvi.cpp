#include <CXO2/States/StateAvi.hpp>
#include <CXO2/States/StatePlanet.hpp>

#include <CXO2/Constants/Identifiers/Sound.hpp>

#include <Genode/Tasks/Action.hpp>
#include <Genode/Tasks/Delay.hpp>
#include <Genode/Tween/Fade.hpp>
#include <Genode/Tasks/Sequence.hpp>
#include <Genode/UI/Image.hpp>

#include <SFML/Audio/Music.hpp>

namespace Cx
{
    using namespace Constants::Identifiers;

    StateAvi::StateAvi(Gx::AudioMixer& mixer) :
        m_mixer(mixer)
    {
    }

    void StateAvi::Initialize()
    {
        State::Initialize();

        auto& director = GetDirector();
        const auto bgm = Instantiate<sf::Music>(Sound::BGM::BG_LOGO);
        m_mixer.Play(*bgm, Sound::Channel::BGM);

        const auto logo = GetChildrenCount() > 0 ? dynamic_cast<Gx::Image*>(GetChildren()[0]) : nullptr;
        if (!logo || logo->GetFrameCount() == 1)
        {
            auto& overlay = Create<Gx::Rectangle>(GetView().getSize());
            overlay.SetColor(sf::Color(0, 0, 0, 255));
            AddChild(overlay);

            Run<Gx::Sequence>([&]
                {
                    director.Present<StatePlanet>();
                },
                Gx::Fade(overlay, 000, sf::seconds(2.5f)),
                Gx::Fade(overlay, 255, sf::seconds(2.5f))
            );
        }
        else
        {
            logo->SetFrame(0);
            Run<Gx::Sequence>([&]
                {
                    director.Present<StatePlanet>();
                },
                Gx::Delay(sf::seconds(2.5f)),
                Gx::Action([=] { logo->SetFrame(logo->GetFrameCount() - 1); }),
                Gx::Delay(sf::seconds(2.5f))
            );
        }
    }

    bool StateAvi::OnAppClose()
    {
        return true;
    }
}
