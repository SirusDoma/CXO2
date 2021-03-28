#ifndef BUTTON_METADATA_HPP
#define BUTTON_METADATA_HPP

#include <O2/IO/Metadata/SpriteMetadata.hpp>

#include <unordered_map>

struct ButtonMetadata : public SpriteMetadata
{
public:
    ButtonMetadata() : m_states() {}
    virtual ~ButtonMetadata() {}

    const std::unordered_map<Gx::Control::State, SpriteMetadata> &GetStates() const { return m_states; }
    void SetState(Gx::Control::State state, SpriteMetadata metadata) { m_states[state] = metadata; }

private:
    std::unordered_map<Gx::Control::State, SpriteMetadata> m_states;
};

#endif