#ifndef GENODE_ANIMATION_HPP
#define GENODE_ANIMATION_HPP

#include <SFML/Graphics/Rect.hpp>

#include <Genode/Entities.hpp>
#include <Genode/SceneGraph.hpp>
#include <Genode/Graphics/Sprite.hpp>

#include <initializer_list>

namespace Gx
{
    class Animation : public Node, public Renderable, public Updatable, public Inputable, public Colorable
    {
    public:
        Animation();
        Animation(Sprite *sprite, const sf::Time& duration, std::initializer_list<sf::IntRect> frames);
        virtual ~Animation();

        void AddFrames(std::initializer_list<sf::IntRect> frames);

        Gx::Sprite *GetSprite() const;
        void SetSprite(Gx::Sprite *sprite);

        const sf::Time& GetDuration() const;
        void SetDuration(const sf::Time& duration);

        const bool& IsLoop() const;
        void SetLoop(const bool& loop);

        virtual const sf::Color& GetColor() const;
        virtual void SetColor(const sf::Color &color);

    protected:
        virtual void Update(double delta);
        virtual sf::RenderStates Render(sf::RenderTarget& target, sf::RenderStates states) const;

    private:
        Sprite *m_sprite;
        sf::Time m_duration;
        sf::Time m_elapsed;
        double m_currentFrame;
        bool m_loop;
        std::vector<sf::IntRect> m_frames;
    };
}

#endif
