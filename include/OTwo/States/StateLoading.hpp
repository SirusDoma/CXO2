#ifndef O2JAM_STATE_LOADING_HPP
#define O2JAM_STATE_LOADING_HPP

#include <OTwo/States/State.hpp>
#include <OTwo/IO/PlayingResourceContext.hpp>

class StateLoading : public State
{
public:
    explicit StateLoading(State &state);
    void Initialize() override;

private:
    void Update(double delta) override;

    const sf::Image *m_cover;
    sf::Texture m_texture;
};

#endif
