#ifndef O2JAM_STATE_TEST_HPP
#define O2JAM_STATE_TEST_HPP

#include <Genode/SceneGraph.hpp>
#include <OTwo/States/State.hpp>

class StateTest : public State
{
public:
    StateTest() = default;
    explicit StateTest(State &&state);

    void OnKeyDown(const sf::Event::KeyEvent ev) override;

    void Initialize() override;
    bool Close(bool quit = false) override;
};

#endif