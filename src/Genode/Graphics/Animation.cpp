#include <Genode/Graphics/Animation.hpp>

namespace Gx
{
    Animation::Animation() :
        m_sprite(nullptr),
        m_duration(sf::Time::Zero),
        m_frames(),
        m_elapsed(sf::Time::Zero),
        m_currentFrame(0),
        m_state(AnimationState::Initial)
    {
    }

    Animation::Animation(Gx::Sprite *sprite, const sf::Time &duration, std::initializer_list<Frame> frames) :
        m_sprite(sprite),
        m_duration(duration),
        m_frames(frames),
        m_elapsed(sf::Time::Zero),
        m_currentFrame(0)
    {
    }

    Animation::~Animation()
    {
    }

    void Animation::AddFrame(const Frame &frame)
    {
        m_frames.push_back(frame);
        if (m_frames.size() == 1)
            SetFrame(0);
    }

    Gx::Sprite *Animation::GetSprite() const
    {
        return m_sprite.get();
    }

    void Animation::SetSprite(Sprite *sprite)
    {
        m_sprite = std::unique_ptr<Sprite>(sprite);
    }

    void Animation::SetDuration(const sf::Time &duration)
    {
        m_duration = duration;
    }

    const sf::Time& Animation::GetDuration() const
    {
        return m_duration;
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

    void Animation::Update(double delta)
    {
        if (GetState() == AnimationState::Completed || GetState() == AnimationState::Stopped)
            return UpdatableContainer::Update(delta);

        m_state    = AnimationState::Playing;
        m_elapsed += sf::milliseconds(delta);

        auto frameTime = sf::milliseconds(m_duration.asMilliseconds() / m_frames.size());
        if (m_elapsed >= frameTime)
        {
            m_elapsed %= frameTime;
            m_currentFrame++;
            if (m_currentFrame >= m_frames.size())
            {
                if (!IsLoop())
                {
                    m_state = AnimationState::Completed;
                    m_elapsed = sf::Time::Zero;

                    return;
                }

                m_currentFrame = 0;
            }

            SetFrame(m_currentFrame);
        }

        UpdatableContainer::Update(delta);
    }

    sf::RenderStates Animation::Render(sf::RenderTarget &target, sf::RenderStates states) const
    {
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
    }

    void Animation::Reset()
    {
        m_state   = AnimationState::Initial;
        m_elapsed = sf::Time::Zero;

        m_currentFrame = 0;
        SetFrame(m_currentFrame);
    }

    void Animation::SetFrame(unsigned int frame)
    {
        if (frame >= 0 && frame < m_frames.size())
        {
            m_sprite->SetTexCoords(m_frames[m_currentFrame].TexCoords);

            m_sprite->SetOrigin(m_frames[m_currentFrame].Origin);
            m_sprite->SetPosition(m_frames[m_currentFrame].Position);
            m_sprite->SetRotation(m_frames[m_currentFrame].Rotation);
            m_sprite->SetScale(m_frames[m_currentFrame].Scale);
        }
    }
}
