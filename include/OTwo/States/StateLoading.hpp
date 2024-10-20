#ifndef O2JAM_STATE_LOADING_HPP
#define O2JAM_STATE_LOADING_HPP

#include <OTwo/States/State.hpp>

#include <OTwo/Core/Chart.hpp>
#include <OTwo/IO/Loaders/Chart/ChartLoader.hpp>
#include <OTwo/Config/GameConfig.hpp>

class GameContext;
class StateLoading : public State
{
public:
    explicit StateLoading(GameContext& game);
    void Initialize() override;

private:
    void Update(double delta) override;
    void OnCoverLoaded(const sf::Image* cover);
    void OnChartLoaded(const Chart* chart);

    sf::Texture m_texture;
    GameContext& m_context;
};

#endif
