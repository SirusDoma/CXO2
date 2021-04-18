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
        struct Frame
        {
            sf::IntRect  TexCoords;
            sf::Vector2f Origin;
            sf::Vector2f Position;
            float        Rotation;
            sf::Vector2f Scale;
        };

        using Sprite::Sprite;
        virtual ~Image();

        virtual const sf::FloatRect GetLocalBounds() const;

        unsigned int GetFrameCount() const;
        const Frame *GetFrame(const std::string &name);
        const Frame *GetFrame(unsigned int index);

        void AddFrame(const std::string &name, const sf::IntRect &texCoords);
        void AddFrame(const std::string &name, const Frame &frame);
        void SetFrame(const std::string &name);
        void SetFrame(unsigned int index);

    protected:
        virtual void Update(double delta);
        virtual sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const;

        virtual void Invalidate();

    private:
        void ApplyFrame(const Frame &frame);

        std::unordered_map<std::string, Frame> m_frames;
    };
}

#endif
