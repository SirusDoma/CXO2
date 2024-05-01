#include <OTwo/States/StatePlaying.hpp>

StatePlaying::StatePlaying(State &state) :
    State(state)
{
}

void StatePlaying::Initialize()
{
    State::Initialize();
}
