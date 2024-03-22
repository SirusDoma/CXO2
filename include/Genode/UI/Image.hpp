#ifndef GENODE_UI_IMAGE_HPP
#define GENODE_UI_IMAGE_HPP

#include <SFML/Graphics/Rect.hpp>

#include <Genode/UI/Control.hpp>
#include <Genode/Graphics/Sprite.hpp>

#include <unordered_map>

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
            std::string  Name;
        };

        using Sprite::Sprite;
        ~Image() override = default;

        sf::FloatRect GetLocalBounds() const override;

        unsigned int GetFrameCount() const;
        Frame *GetFrame(const std::string &name) const;
        Frame *GetFrame(unsigned int index) const;
        Frame *GetCurrentFrame() const;

        bool ContainsFrame(const std::string &name) const;
        bool ContainsFrame(unsigned int index) const;

        void AddFrame(const std::string &name, const sf::IntRect &texCoords);
        void AddFrame(const std::string &name, const Frame &frame);

        void SetFrame(const std::string &name);
        void SetFrame(unsigned int index);

        void NextFrame();
        void PreviousFrame();

    protected:
        void Update(double delta) override;
        sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const override;

        void Invalidate() override;

    private:
        void ApplyFrame(Frame &frame);

        mutable unsigned int m_currentIndex = 0;
        Frame *m_currentFrame = nullptr;
        std::unordered_map<std::string, Frame> m_frames;
        std::unordered_map<unsigned int, std::string> m_indices;
    };
}

#endif
