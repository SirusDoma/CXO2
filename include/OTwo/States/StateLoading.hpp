#ifndef O2JAM_STATE_LOADING_HPP
#define O2JAM_STATE_LOADING_HPP

#include <OTwo/States/State.hpp>

#include <OTwo/Core/Chart.hpp>
#include <OTwo/IO/Loaders/Chart/ChartLoader.hpp>
#include <OTwo/Config/GameConfig.hpp>

class SessionContext;
class GameContext;
class StateLoading : public State
{
public:
    StateLoading(const SessionContext& session, GameContext& game, GameConfig& config);
    void Initialize() override;

private:
    void Update(double delta) override;
    void OnCoverLoaded(const sf::Image* cover);
    void OnChartLoaded(const Chart* chart);

    sf::Texture m_texture;
    const SessionContext& m_session;
    GameContext& m_game;
    GameConfig& m_config;
};

#endif
