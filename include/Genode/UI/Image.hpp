#ifndef GENODE_UI_IMAGE_HPP
#define GENODE_UI_IMAGE_HPP

#include <SFML/Graphics/Rect.hpp>

#include <Genode/UI/Control.hpp>
#include <Genode/Graphics/Sprite.hpp>

#include <unordered_map>

namespace Gx
{
    class Image : public virtual Control, public virtual Sprite
    {
    public:
        using Sprite::Sprite;
        virtual ~Image();

        virtual const sf::FloatRect GetLocalBounds() const;

        void AddFrame(const std::string &name, const sf::IntRect &texCoords);
        void SetFrame(const std::string &name);

    protected:
        virtual void Update(double delta);
        virtual sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const;

        virtual void Invalidate();
    private:
        std::unordered_map<std::string, sf::IntRect> m_frames;
    };
}

#endif
