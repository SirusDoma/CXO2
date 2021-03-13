#include <Genode/Graphics/Animation.hpp>
#include <iostream>

namespace Gx
{
    Animation::Animation() :
        m_sprite(nullptr),
        m_duration(sf::Time::Zero),
        m_frames(),
        m_elapsed(sf::Time::Zero),
        m_currentFrame(0)
    {
    }

    Animation::Animation(Gx::Sprite *sprite, const sf::Time &duration, std::initializer_list<sf::IntRect> frames) :
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

    void Animation::AddFrames(std::initializer_list<sf::IntRect> frames)
    {
        for (auto frame : frames)
            m_frames.push_back(frame);
    }

    Gx::Sprite *Animation::GetSprite() const
    {
        return m_sprite;
    }

    void Animation::SetSprite(Gx::Sprite *sprite)
    {
        if (m_sprite)
            delete m_sprite;

        m_sprite = sprite;
    }

    void Animation::SetDuration(const sf::Time &duration)
    {
        m_duration = duration;
    }

    const sf::Time& Animation::GetDuration() const
    {
        return m_duration;
    }

    void Animation::SetLoop(const bool &loop)
    {
        m_loop = loop;
    }

    const bool &Animation::IsLoop() const
    {
        return m_loop;
    }

    void Animation::Update(double delta)
    {
        m_elapsed += sf::milliseconds(delta);
        if (m_elapsed >= m_duration)
        {
            if (IsLoop())
                m_elapsed = sf::milliseconds(m_elapsed.asMilliseconds() % m_duration.asMilliseconds());
            else
                return;
        }

        auto frameTime = sf::milliseconds(m_duration.asMilliseconds() / m_frames.size());
        if (m_elapsed >= frameTime)
        {
            m_elapsed %= frameTime;
            m_currentFrame++;
            if (m_currentFrame >= m_frames.size())
              m_currentFrame = 0;

            m_sprite->SetTexCoords(m_frames[m_currentFrame]);
        }
    }

    sf::RenderStates Animation::Render(sf::RenderTarget &target, sf::RenderStates states) const
    {
        states.transform *= GetTransform();
        if (!m_sprite)
            return states;

        auto drawable = static_cast<sf::Drawable*>(m_sprite);
        target.draw(*drawable, states);

        return states;
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
}