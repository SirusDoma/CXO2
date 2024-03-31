#ifndef GENODE_GRAPHICS_ANIMATION_HPP
#define GENODE_GRAPHICS_ANIMATION_HPP

#include <SFML/Graphics/Rect.hpp>

#include <Genode/SceneGraph/Node.hpp>
#include <Genode/SceneGraph/RenderableContainer.hpp>
#include <Genode/SceneGraph/UpdatableContainer.hpp>
#include <Genode/SceneGraph/InputableContainer.hpp>
#include <Genode/Graphics/Sprite.hpp>

#include <initializer_list>
#include <functional>

namespace Gx
{
    class Animation : public virtual Node, public virtual RenderableContainer, public virtual UpdatableContainer, public virtual InputableContainer, public Colorable
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
        Animation(Sprite &sprite, const sf::Time& duration, std::initializer_list<Frame> frames);
        ~Animation() override = default;

        template<typename... Args>
        void AddFrame(const Frame &first, Args... args);
        void AddFrame(const Frame &frame);
        unsigned int GetFrameCount() const;

        Gx::Sprite &GetSprite() const;
        void SetSprite(Gx::Sprite &sprite);

        const sf::Time& GetDuration() const;
        void SetDuration(const sf::Time& duration);

        unsigned int GetRepeatCount() const;
        void SetRepeatCount(unsigned int repeatCount);

        bool IsLoop() const;
        void SetLoop(bool loop);

        const sf::Color& GetColor() const override;
        void SetColor(const sf::Color &color) override;

        AnimationState GetState() const;
        void SetAnimationCallback(const std::function<void(Animation &)> &animationCallback);

        bool GetVisible() const;
        void SetVisible(bool visible);

        virtual void Stop();
        virtual void Reset();

        void Update(double delta) override;
        RenderStates Render(sf::RenderTarget& target, RenderStates states) const override;

    private:
        void SetFrame(unsigned int index);

        std::unique_ptr<Sprite> m_sprite;
        AnimationState m_state;
        bool m_visible;

        sf::Time m_duration;
        sf::Time m_elapsed;
        unsigned int m_currentFrame;
        unsigned int m_currentRepeat;
        unsigned int m_repeatCount;
        bool m_loop;
        std::vector<Frame> m_frames;

        std::function<void(Animation &sender)> m_animationCallback;
    };
}

#include <Genode/Graphics/Animation.inl>
#endif
