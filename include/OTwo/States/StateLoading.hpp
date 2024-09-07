#ifndef O2JAM_STATE_LOADING_HPP
#define O2JAM_STATE_LOADING_HPP

#include <OTwo/States/State.hpp>
#include <OTwo/IO/Loaders/Chart/ChartLoader.hpp>
#include <OTwo/Core/Chart.hpp>

class StateLoading : public State
{
public:
    StateLoading() = default;
    void Initialize() override;

private:
    void Update(double delta) override;
    void OnCoverLoaded(const sf::Image *cover);
    void OnChartLoaded(const Chart *chart);

    sf::Texture m_texture;
    ChartLoader m_loader;
};

#endif
