#ifndef GENODE_ANIMATION_HPP
#define GENODE_ANIMATION_HPP

#include <SFML/Graphics/Rect.hpp>

#include <Genode/Entities.hpp>
#include <Genode/SceneGraph.hpp>
#include <Genode/Graphics/Sprite.hpp>

#include <initializer_list>

namespace Gx
{
    class Animation : public virtual Node, public RenderableContainer, public UpdatableContainer, public InputableContainer, public Colorable
    {
    public:
        Animation();
        Animation(Sprite *sprite, const sf::Time& duration, std::initializer_list<sf::IntRect> frames);
        virtual ~Animation();

        enum AnimationState
        {
            Initial,
            Playing,
            Stopped,
            Completed
        };

        template<typename... Args>
        void AddFrame(sf::IntRect first, Args... args);
        void AddFrame(const sf::IntRect &frame);

        Gx::Sprite *GetSprite() const;
        void SetSprite(Gx::Sprite *sprite);

        const sf::Time& GetDuration() const;
        void SetDuration(const sf::Time& duration);

        bool IsLoop() const;
        void SetLoop(bool loop);

        virtual const sf::Color& GetColor() const;
        virtual void SetColor(const sf::Color &color);

        const AnimationState GetState() const;

        virtual void Stop();
        virtual void Reset();

    protected:
        virtual void Update(double delta);
        virtual sf::RenderStates Render(sf::RenderTarget& target, sf::RenderStates states) const;

    private:
        Sprite *m_sprite;
        AnimationState m_state;

        sf::Time m_duration;
        sf::Time m_elapsed;
        double m_currentFrame;
        bool m_loop;
        std::vector<sf::IntRect> m_frames;
    };
}

#include <Genode/Graphics/Animation.inl>
#endif
