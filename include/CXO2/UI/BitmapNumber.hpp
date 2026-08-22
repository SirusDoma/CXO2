#pragma once

#include <Genode/Graphics/Animation.hpp>
#include <Genode/Graphics/Sprite.hpp>
#include <CXO2/UI/Control.hpp>

#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Time.hpp>

#include <unordered_map>

namespace Cx
{
    class BitmapNumber : public virtual Control, public virtual Gx::Colorable
    {
    public:
        enum class Alignment { None, Left, Center, Right };

        BitmapNumber();
        explicit BitmapNumber(const sf::Texture& texture);

        [[nodiscard]] sf::FloatRect GetLocalBounds() const override;

        [[nodiscard]] const sf::Texture* GetTexture() const;
        void SetTexture(const sf::Texture& texture);

        [[nodiscard]] const sf::Color& GetColor() const override;
        void SetColor(const sf::Color& color) override;

        [[nodiscard]] float GetKerning() const;
        void SetKerning(float value);

        [[nodiscard]] int GetDigitCount() const;
        void SetDigitCount(int count);

        [[nodiscard]] const sf::Time& GetAnimationDuration(unsigned int digit) const;
        void SetAnimationDuration(const sf::Time& duration);
        void SetAnimationDuration(unsigned int digit, const sf::Time& duration);

        void SetDigitFrames(unsigned int digit, const std::vector<sf::IntRect>& texCoords);
        void SetDigitFrame(unsigned int digit, sf::IntRect texCoords);
        void SetDigitsSize(sf::Vector2u size);

        [[nodiscard]] unsigned long long GetValue() const;
        void SetValue(unsigned long long value);

        [[nodiscard]] Alignment GetAlignment() const;
        void SetAlignment(Alignment alignment);

        [[nodiscard]] Gx::BlendMode GetBlendMode() const;
        void SetBlendMode(Gx::BlendMode blendMode);

        [[nodiscard]] Gx::Animation::AnimationState GetAnimationState() const;
        void SetAnimationCallback(const std::function<void(BitmapNumber&, Control::Event&)> &animationCallback);

        void Stop();
        void Reset();

    protected:
        void Update(const sf::Time& delta) override;
        Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;
        void Invalidate() override;

    private:
        sf::VertexArray   m_vertices;
        const sf::Texture* m_texture;
        Gx::BlendMode         m_blendMode;
        Alignment         m_alignment;

        Gx::Animation::AnimationState m_state;
        unsigned long long m_value;
        unsigned int m_digitCount;
        float m_width, m_height, m_kerning;
        bool m_needUpdate;

        std::unordered_map<unsigned int, sf::Time> m_durations;
        std::unordered_map<unsigned int, sf::Time> m_elapseds;
        std::unordered_map<unsigned int, unsigned int> m_frames;

        std::function<void(BitmapNumber&, Control::Event&)> m_callback;
        std::unordered_map<unsigned int, std::vector<sf::IntRect>> m_texCoords;
    };
}
