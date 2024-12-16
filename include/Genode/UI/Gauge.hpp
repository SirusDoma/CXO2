#pragma once

#include <Genode/Graphics/Animation.hpp>
#include <Genode/UI/Control.hpp>

#include <SFML/Graphics.hpp>

namespace Gx
{
    class Gauge : public virtual Control, public virtual Colorable
    {
    public:
        enum class Orientation { Horizontal, Vertical };

        Gauge();
        explicit Gauge(const sf::Texture& texture);
        Gauge(const sf::Texture& texture, const sf::IntRect& texCoords);

        sf::FloatRect GetLocalBounds() const override;

        bool IsFlickering() const;
        void SetFlickering(bool flicker);

        bool IsSlanted() const;
        void SetSlanted(bool slanted);

        Animation::AnimationState GetAnimationState() const;

        template<typename... Args>
        void AddAnimationFrame(const Animation::Frame& first, const Args&... args);
        void AddAnimationFrame(const Animation::Frame& frame);
        unsigned int GetAnimationFrameCount() const;

        const sf::Time& GetAnimationDuration() const;
        void SetAnimationDuration(const sf::Time& duration);

        const sf::Texture* GetTexture() const;
        void SetTexture(const sf::Texture& texture);

        const sf::IntRect& GetTexCoords() const;
        void SetTexCoords(const sf::IntRect& texCoords);

        const sf::Color& GetColor() const override;
        void SetColor(const sf::Color& color) override;

        Orientation GetOrientation() const;
        void SetOrientation(const Orientation& orientation);

        float GetMaximumValue() const;
        void SetMaximumValue(float max);

        float GetValue() const;
        void SetValue(float value);

    protected:
        void Update(double delta) override;
        RenderStates Render(RenderSurface& surface, RenderStates states) const override;

        void Invalidate() override;

    private:
        sf::VertexArray    m_vertices;
        const sf::Texture* m_texture;
        sf::IntRect        m_texCoords;
        Orientation        m_orientation;
        float              m_value, m_maximum;
        bool               m_flicker, m_flickerActivate, m_slanted;

        Animation::AnimationState m_animationState;
        unsigned int m_currentFrame;
        sf::Time m_animationDuration;
        sf::Time m_animationElapsed;
        std::vector<Animation::Frame> m_frames;
    };

}

#include <Genode/UI/Gauge.inl>
