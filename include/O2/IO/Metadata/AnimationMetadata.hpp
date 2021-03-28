#ifndef ANIMATION_METADATA_HPP
#define ANIMATION_METADATA_HPP

#include <O2/IO/Metadata/SpriteMetadata.hpp>
#include <vector>

class AnimationMetadata : public SpriteMetadata
{
public:
    AnimationMetadata() : m_frames() {}
    virtual ~AnimationMetadata() {}

    bool isLoop() const { return m_isLoop; }
    void SetLoop(bool isLoop) { m_isLoop = isLoop; }

    const sf::Time &GetDuration() const { return m_duration; }
    void SetDuration(const sf::Time &duration) { m_duration = duration; }

    const std::vector<sf::IntRect> &GetFrames() const { return m_frames; }
    void AddFrame(const sf::IntRect &frame) { m_frames.push_back(frame); }

private:
    bool     m_isLoop;
    sf::Time m_duration;

    std::vector<sf::IntRect> m_frames;
};

#endif