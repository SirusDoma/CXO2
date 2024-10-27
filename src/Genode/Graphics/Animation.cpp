#include <Genode/Graphics/Animation.hpp>

namespace Gx
{
    Animation::Animation() :
        Gx::Sprite(),
        m_state(AnimationState::Initial),
        m_duration(sf::Time::Zero),
        m_elapsed(sf::Time::Zero),
        m_speed(1.f),
        m_currentFrame(0),
        m_currentRepeat(0),
        m_repeatCount(0),
        m_loop(false),
        m_frames(),
        m_animationCallback()
    {
    }

    Animation::Animation(const sf::Texture& texture, const sf::Time& duration, const std::initializer_list<Frame> frames) :
        Gx::Sprite(texture),
        m_state(AnimationState::Initial),
        m_duration(duration),
        m_elapsed(sf::Time::Zero),
        m_speed(1.f),
        m_currentFrame(0),
        m_currentRepeat(0),
        m_repeatCount(0),
        m_loop(false),
        m_frames(frames),
        m_animationCallback()
    {
    }

    void Animation::AddFrame(const Frame& frame)
    {
        m_frames.push_back(frame);
        if (m_frames.size() == 1)
        {
            m_currentFrame = 0;
            SetFrame(m_currentFrame);
        }
    }

    void Animation::SetDuration(const sf::Time& duration)
    {
        m_duration = duration;
    }

    float Animation::GetSpeed() const
    {
        return m_speed;
    }

    void Animation::SetSpeed(const float speed)
    {
        m_speed = speed;
    }

    const sf::Time& Animation::GetDuration() const
    {
        return m_duration;
    }

    unsigned int Animation::GetRepeatCount() const
    {
        return m_repeatCount;
    }

    void Animation::SetRepeatCount(const unsigned int repeatCount)
    {
        m_repeatCount = repeatCount;
        m_currentRepeat = 0;
    }

    void Animation::SetLoop(const bool loop)
    {
        m_loop = loop;
    }

    bool Animation::IsLoop() const
    {
        return m_loop;
    }

    Animation::AnimationState Animation::GetState() const
    {
        return m_state;
    }

    void Animation::SetAnimationCallback(const std::function<void(Animation &)> &animationCallback)
    {
        m_animationCallback = animationCallback;
    }

    void Animation::Update(const double delta)
    {
        if (GetState() == AnimationState::Completed || GetState() == AnimationState::Stopped)
            return UpdatableContainer::Update(delta);

        const bool trigger = m_state != AnimationState::Playing;
        m_state      = AnimationState::Playing;
        m_elapsed   += sf::milliseconds(static_cast<int>(delta));

        if (trigger)
        {
            if (m_animationCallback)
                m_animationCallback(*this);
        }

        if (const auto frameTime = sf::milliseconds( static_cast<int>(m_duration.asMilliseconds() / m_speed) / static_cast<int>(m_frames.size())); m_elapsed >= frameTime)
        {
            m_elapsed = frameTime == sf::Time::Zero ? sf::Time::Zero : m_elapsed % frameTime;
            m_currentFrame++;
            if (m_currentFrame >= m_frames.size())
            {
                if (!IsLoop() && m_currentRepeat >= m_repeatCount)
                {
                    m_state         = AnimationState::Completed;
                    m_elapsed       = sf::Time::Zero;
                    m_currentRepeat = 0;
                    if (m_animationCallback)
                        m_animationCallback(*this);

                    return;
                }

                if (m_currentRepeat < m_repeatCount)
                    m_currentRepeat++;

                m_currentFrame = 0;
            }

            SetFrame(m_currentFrame);
        }

        UpdatableContainer::Update(delta);
    }

    RenderStates Animation::Render(RenderSurface& surface, RenderStates states) const
    {
        return Sprite::Render(surface, states);
    }

    void Animation::Stop()
    {
        m_state   = AnimationState::Stopped;
        m_elapsed = sf::Time::Zero;

        if (m_animationCallback)
            m_animationCallback(*this);
    }

    void Animation::Reset()
    {
        m_state   = AnimationState::Initial;
        m_elapsed = sf::Time::Zero;
        m_currentRepeat = 0;

        if (m_animationCallback)
            m_animationCallback(*this);

        m_currentFrame = 0;
        SetFrame(m_currentFrame);
    }

    void Animation::SetFrame(const unsigned int index)
    {
        if (index < m_frames.size())
        {
            const auto& frame = m_frames[index];
            SetTexCoords(frame.TexCoords);

            if (frame.Origin.has_value())
                SetOrigin(frame.Origin.value());

            if (frame.Position.has_value())
                SetPosition(frame.Position.value());

            if (frame.Rotation.has_value())
                SetRotation(frame.Rotation.value());

            if (frame.Scale.has_value())
                SetScale(frame.Scale.value());
        }
    }

    unsigned int Animation::GetFrameCount() const
    {
        return m_frames.size();
    }

    Animation::Frame& Animation::GetFrame(const unsigned int index)
    {
        return m_frames[index];
    }
}
