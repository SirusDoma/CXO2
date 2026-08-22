#pragma once

#include <Genode/Graphics/Animation.hpp>
#include <CXO2/UI/Control.hpp>

#include <SFML/Graphics.hpp>

namespace Cx
{
    class Gauge : public virtual Control, public virtual Gx::Colorable
    {
    public:
        enum class Orientation { Horizontal, Vertical };

        Gauge();
        explicit Gauge(const sf::Texture& texture);
        Gauge(const sf::Texture& texture, const sf::IntRect& texCoords);

        [[nodiscard]] sf::FloatRect GetLocalBounds() const override;

        [[nodiscard]] bool IsFlickering() const;
        void SetFlickering(bool flicker);

        [[nodiscard]] bool IsSlanted() const;
        void SetSlanted(bool slanted);

        [[nodiscard]] Gx::Animation::AnimationState GetAnimationState() const;

        template<typename... Args>
        void AddAnimationFrame(const Gx::Animation::Frame& first, const Args&... args);
        void AddAnimationFrame(const Gx::Animation::Frame& frame);
        [[nodiscard]] unsigned int GetAnimationFrameCount() const;

        [[nodiscard]] const sf::Time& GetAnimationDuration() const;
        void SetAnimationDuration(const sf::Time& duration);

        [[nodiscard]] const sf::Texture* GetTexture() const;
        void SetTexture(const sf::Texture& texture);

        [[nodiscard]] const sf::IntRect& GetTexCoords() const;
        void SetTexCoords(const sf::IntRect& texCoords);

        [[nodiscard]] const sf::Color& GetColor() const override;
        void SetColor(const sf::Color& color) override;

        [[nodiscard]] Orientation GetOrientation() const;
        void SetOrientation(const Orientation& orientation);

        [[nodiscard]] float GetMaximumValue() const;
        void SetMaximumValue(float max);

        [[nodiscard]] float GetValue() const;
        void SetValue(float value);

    protected:
        void Update(const sf::Time& delta) override;
        Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

        void Invalidate() override;

    private:
        sf::VertexArray    m_vertices;
        const sf::Texture* m_texture;
        sf::IntRect        m_texCoords;
        Orientation        m_orientation;
        float              m_value, m_maximum;
        bool               m_flicker, m_flickerActivate, m_slanted;

        Gx::Animation::AnimationState m_animationState;
        unsigned int m_currentFrame;
        sf::Time m_animationDuration;
        sf::Time m_animationElapsed;
        std::vector<Gx::Animation::Frame> m_frames;
    };

}

#include <CXO2/UI/Gauge.inl>
