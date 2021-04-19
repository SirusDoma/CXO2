#ifndef GENODE_UI_IMAGE_HPP
#define GENODE_UI_IMAGE_HPP

#include <SFML/Graphics/Rect.hpp>

#include <Genode/UI/Control.hpp>
#include <Genode/Graphics/Sprite.hpp>

#include <map>

namespace Gx
{
    class Image : public Control, public Sprite
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
        const Frame *GetFrame(const std::string &name) const;
        const Frame *GetFrame(unsigned int index) const;

        bool ContainsFrame(const std::string &name) const;
        bool ContainsFrame(unsigned int index) const;

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

        std::map<std::string, Frame> m_frames;
    };
}

#endif
