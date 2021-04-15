#ifndef GENODE_GRAPHICS_ANIMATION_HPP
#define GENODE_GRAPHICS_ANIMATION_HPP

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
        enum class AnimationState
        {
            Initial,
            Playing,
            Stopped,
            Completed
        };

        struct Frame
        {
            sf::IntRect  TexCoords;
            sf::Vector2f Origin;
            sf::Vector2f Position;
            float        Rotation;
            sf::Vector2f Scale;
        };

        Animation();
        Animation(Sprite *sprite, const sf::Time& duration, std::initializer_list<Frame> frames);
        virtual ~Animation();

        template<typename... Args>
        void AddFrame(const Frame &first, Args... args);
        void AddFrame(const Frame &frame);

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

        virtual void Update(double delta);
        virtual sf::RenderStates Render(sf::RenderTarget& target, sf::RenderStates states) const;

    private:
        void SetFrame(unsigned int index);

        std::unique_ptr<Sprite> m_sprite;
        AnimationState m_state;

        sf::Time m_duration;
        sf::Time m_elapsed;
        unsigned int m_currentFrame;
        bool m_loop;
        std::vector<Frame> m_frames;
    };
}

#include <Genode/Graphics/Animation.inl>
#endif
