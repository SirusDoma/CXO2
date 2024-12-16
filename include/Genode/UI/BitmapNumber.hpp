#pragma once

#include <Genode/Graphics/Animation.hpp>
#include <Genode/Graphics/Sprite.hpp>
#include <Genode/UI/Control.hpp>

#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Time.hpp>

#include <unordered_map>

namespace Gx
{
    class BitmapNumber : public virtual Control, public virtual Colorable
    {
    public:
        enum class Alignment { None, Left, Center, Right };

        BitmapNumber();
        explicit BitmapNumber(const sf::Texture& texture);

        sf::FloatRect GetLocalBounds() const override;

        const sf::Texture* GetTexture() const;
        void SetTexture(const sf::Texture& texture);

        const sf::Color& GetColor() const override;
        void SetColor(const sf::Color& color) override;

        float GetKerning() const;
        void SetKerning(float value);

        int GetDigitCount() const;
        void SetDigitCount(int count);

        const sf::Time& GetAnimationDuration(unsigned int digit) const;
        void SetAnimationDuration(const sf::Time& duration);
        void SetAnimationDuration(unsigned int digit, const sf::Time& duration);

        void SetDigitFrames(unsigned int digit, const std::vector<sf::IntRect>& texCoords);
        void SetDigitFrame(unsigned int digit, sf::IntRect texCoords);
        void SetDigitsSize(sf::Vector2u size);

        unsigned long long GetValue() const;
        void SetValue(unsigned long long value);

        Alignment GetAlignment() const;
        void SetAlignment(Alignment alignment);

        BlendMode GetBlendMode() const;
        void SetBlendMode(Gx::BlendMode blendMode);

        Animation::AnimationState GetAnimationState() const;
        void SetAnimationCallback(const std::function<void(BitmapNumber&)> &animationCallback);

        void Stop();
        void Reset();

    protected:
        void Update(double delta) override;
        RenderStates Render(RenderSurface& surface, RenderStates states) const override;
        void Invalidate() override;

    private:
        sf::VertexArray   m_vertices;
        const sf::Texture* m_texture;
        BlendMode         m_blendMode;
        Alignment         m_alignment;

        Animation::AnimationState m_state;
        unsigned long long m_value;
        unsigned int m_digitCount;
        float m_width, m_height, m_kerning;
        bool m_needUpdate;

        std::unordered_map<unsigned int, sf::Time> m_durations;
        std::unordered_map<unsigned int, sf::Time> m_elapseds;
        std::unordered_map<unsigned int, unsigned int> m_frames;

        std::function<void(BitmapNumber&)> m_callback;
        std::unordered_map<unsigned int, std::vector<sf::IntRect>> m_texCoords;
    };
}
