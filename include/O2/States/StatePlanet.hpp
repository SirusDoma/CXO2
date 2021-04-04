#ifndef STATE_PLANET_HPP
#define STATE_PLANET_HPP

#include <SFML/Audio.hpp>

#include <Genode/Graphics.hpp>
#include <Genode/SceneGraph.hpp>
#include <Genode/Tasks/Sequence.hpp>

#include <O2/States/State.hpp>
#include <O2/Data/Planet.hpp>

#include <O2/States/Components/Planet/ChannelBoard.hpp>

class StatePlanet : public State
{
public:
    StatePlanet(bool fadeIn = true);
    virtual void Initialize();

private:
    bool IsConnecting();

    void OnEnterPlanet(Planet planet);
    void OnEnterChannel(Planet planet, ChannelInfo channel);

    std::unique_ptr<ChannelBoard>    m_channelBoard;
    std::unique_ptr<Gx::UiContainer> m_container;

    sf::Music  *m_bgm;
    Gx::Dialog *m_dialogInfo;

    bool m_useFadeIn;
    bool m_connecting;
};

#endif
