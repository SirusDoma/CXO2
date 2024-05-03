#ifndef O2JAM_STATE_PLAYING_HPP
#define O2JAM_STATE_PLAYING_HPP

#include <OTwo/States/State.hpp>
#include <Genode/SceneGraph.hpp>
#include <Genode/Graphics/Animation.hpp>

class StatePlaying : public State
{
public:
    StatePlaying() = default;
    explicit StatePlaying(State &state);

    void Initialize() override;

    void SetNoteClick(Gx::ResourcePtr<Gx::Animation> click);
};


#endif
