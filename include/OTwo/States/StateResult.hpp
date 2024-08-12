#ifndef O2JAM_STATE_RESULT_HPP
#define O2JAM_STATE_RESULT_HPP

#include <OTwo/States/State.hpp>
#include <OTwo/IO/PlayingResourceContext.hpp>
#include <OTwo/Core/Chart.hpp>

class StateResult : public State
{
public:
    explicit StateResult(State &&state);
    void Initialize() override;

private:
    void Update(double delta) override;
};

#endif
