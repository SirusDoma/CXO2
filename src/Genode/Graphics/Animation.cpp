#include <Genode/Graphics/Animation.hpp>

namespace Gx
{
    Animation::Animation() :
        m_sprite(std::make_unique<Gx::Sprite>()),
        m_frames(),
        m_duration(sf::Time::Zero),
        m_elapsed(sf::Time::Zero),
        m_currentFrame(0),
        m_currentRepeat(0),
        m_repeatCount(0),
        m_state(AnimationState::Initial),
        m_visible(true),
        m_loop(false),
        m_animationCallback()
    {
    }

    Animation::Animation(Gx::Sprite &sprite, const sf::Time &duration, std::initializer_list<Frame> frames) :
        m_sprite(&sprite),
        m_frames(frames),
        m_duration(duration),
        m_elapsed(sf::Time::Zero),
        m_currentFrame(0),
        m_currentRepeat(0),
        m_repeatCount(0),
        m_visible(true),
        m_loop(false),
        m_animationCallback()
    {
    }

    void Animation::AddFrame(const Frame &frame)
    {
        m_frames.push_back(frame);
        if (m_frames.size() == 1)
        {
            m_currentFrame = 0;
            SetFrame(m_currentFrame);
        }
    }

    Gx::Sprite &Animation::GetSprite() const
    {
        return *m_sprite.get();
    }

    void Animation::SetSprite(Sprite &sprite)
    {
        m_sprite = std::make_unique<Sprite>(sprite);
    }

    void Animation::SetDuration(const sf::Time &duration)
    {
        m_duration = duration;
    }

    const sf::Time& Animation::GetDuration() const
    {
        return m_duration;
    }

    unsigned int Animation::GetRepeatCount() const
    {
        return m_repeatCount;
    }

    void Animation::SetRepeatCount(unsigned int repeatCount)
    {
        m_repeatCount = repeatCount;
        m_currentRepeat = 0;
    }

    void Animation::SetLoop(bool loop)
    {
        m_loop = loop;
    }

    bool Animation::IsLoop() const
    {
        return m_loop;
    }

    void Animation::SetColor(const sf::Color &color)
    {
        if (m_sprite)
            m_sprite->SetColor(color);
    }

    const sf::Color &Animation::GetColor() const
    {
        if (m_sprite)
            return m_sprite->GetColor();

        return sf::Color::White;
    }

    const Animation::AnimationState Animation::GetState() const
    {
        return m_state;
    }

    void Animation::SetAnimationCallback(const std::function<void(Animation &)> &animationCallback)
    {
        m_animationCallback = animationCallback;
    }

    bool Animation::GetVisible() const
    {
        return m_visible;
    }

    void Animation::SetVisible(bool visible)
    {
        m_visible = visible;
    }

    void Animation::Update(double delta)
    {
        if (GetState() == AnimationState::Completed || GetState() == AnimationState::Stopped)
            return UpdatableContainer::Update(delta);

        bool trigger = m_state != AnimationState::Playing;
        m_state      = AnimationState::Playing;
        m_elapsed   += sf::milliseconds(delta);

        if (trigger)
        {
            if (m_animationCallback)
                m_animationCallback(*this);
        }

        auto frameTime = sf::milliseconds(m_duration.asMilliseconds() / m_frames.size());
        if (m_elapsed >= frameTime)
        {
            m_elapsed %= frameTime;
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

    RenderStates Animation::Render(sf::RenderTarget &target, RenderStates states) const
    {
        if (!m_visible)
            return states;

        states.transform *= GetTransform();
        if (!m_sprite)
            return states;

        target.draw(*m_sprite.get(), states);
        return RenderableContainer::Render(target, states);
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

    void Animation::SetFrame(unsigned int frame)
    {
        if (frame < m_frames.size())
        {
            m_sprite->SetTexCoords(m_frames[frame].TexCoords);

            m_sprite->SetOrigin(m_frames[frame].Origin);
            m_sprite->SetPosition(m_frames[frame].Position);
            m_sprite->SetRotation(m_frames[frame].Rotation);
            m_sprite->SetScale(m_frames[frame].Scale);
        }
    }

    unsigned int Animation::GetFrameCount() const
    {
        return m_frames.size();
    }
}
