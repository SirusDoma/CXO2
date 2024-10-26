#pragma once

#include <SFML/Graphics/Rect.hpp>

#include <Genode/UI/Control.hpp>
#include <Genode/Graphics/Sprite.hpp>

#include <unordered_map>

namespace Gx
{
    class Image : public Control, public Sprite
    {
    public:
        struct Frame final
        {
            sf::IntRect  TexCoords = sf::IntRect();
            sf::Vector2f Origin    = sf::Vector2f();
            sf::Vector2f Position  = sf::Vector2f();
            float        Rotation  = 0.f;
            sf::Vector2f Scale     = sf::Vector2f(1.f, 1.f);
        };

        using Sprite::Sprite;

        sf::FloatRect GetLocalBounds() const override;
        sf::FloatRect GetGlobalBounds() const override;

        unsigned int GetFrameCount() const;
        const Frame* GetFrame(const std::string& name) const;
        const Frame* GetFrame(unsigned int index) const;
        const Frame* GetCurrentFrame() const;
        const std::string& GetCurrentFrameName() const;
        const unsigned int &GetCurrentFrameIndex() const;

        bool ContainsFrame(const std::string& name) const;
        bool ContainsFrame(unsigned int index) const;

        void AddFrame(const std::string& name, const sf::IntRect& texCoords);
        void AddFrame(const std::string& name, const Frame& frame);
        void SetFrame(const std::string& name);
        void SetFrame(unsigned int index);

    protected:
        void Update(double delta) override;
        RenderStates Render(RenderSurface& surface, RenderStates states) const override;

        void Invalidate() override;

    private:
        void ApplyFrame(const Frame& frame);

        std::string m_frameName;
        unsigned int m_frameIndex;
        const Frame* m_currentFrame;
        std::vector<std::string> m_indices;
        std::unordered_map<std::string, Frame> m_frames;
    };
}
