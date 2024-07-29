#ifndef O2JAM_STATE_LOADING_HPP
#define O2JAM_STATE_LOADING_HPP

#include <OTwo/States/State.hpp>
#include <OTwo/IO/PlayingResourceContext.hpp>
#include <OTwo/Chart/Chart.hpp>

class StateLoading : public State
{
public:
    explicit StateLoading(State &&state);
    void Initialize() override;

private:
    void Update(double delta) override;
    void OnCoverLoaded(const sf::Image *cover);
    void OnChartLoaded(const Chart *chart);

    sf::Texture m_texture;
};

#endif
