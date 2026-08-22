#pragma once

#include <SFML/Graphics/Rect.hpp>

#include <CXO2/UI/Control.hpp>
#include <Genode/Graphics/Sprite.hpp>

#include <unordered_map>

namespace Cx
{
    class Image : public Control, public virtual Gx::Colorable
    {
    public:
        enum class SizeMode
        {
            Normal  = 0,
            Center  = 1,
            Stretch = 2
        };

        struct Frame final
        {
            sf::IntRect  TexCoords = sf::IntRect();
            sf::Vector2f Origin    = sf::Vector2f();
            sf::Vector2f Position  = sf::Vector2f();
            float        Rotation  = 0.f;
            sf::Vector2f Scale     = sf::Vector2f(1.f, 1.f);
        };

        Image();
        explicit Image(const sf::Texture& texture);
        Image(const sf::Texture& texture, const sf::IntRect& rectangle);

        [[nodiscard]] sf::FloatRect GetLocalBounds() const override;
        [[nodiscard]] sf::FloatRect GetGlobalBounds() const override;

        [[nodiscard]] const sf::Texture* GetTexture() const;
        [[nodiscard]] const sf::IntRect& GetTexCoords() const;
        [[nodiscard]] const sf::Color& GetColor() const override;
        [[nodiscard]] SizeMode GetSizeMode() const;
        [[nodiscard]] Gx::BlendMode GetBlendMode() const;

        void SetTexture(const sf::Texture& texture, bool resetRect = false);
        void SetTexCoords(const sf::IntRect& rectangle);
        void SetLocalBounds(const sf::FloatRect& bounds);
        void SetColor(const sf::Color& color) override;
        void SetSizeMode(SizeMode sizeMode);
        void SetBlendMode(Gx::BlendMode blendMode);

        [[nodiscard]] unsigned int GetFrameCount() const;

        void AddFrame(const std::string& name, const sf::IntRect& texCoords);
        void AddFrame(const std::string& name, const Frame& frame);

        [[nodiscard]] const Frame* GetFrame(const std::string& name) const;
        [[nodiscard]] const Frame* GetFrame(unsigned int index) const;
        [[nodiscard]] const Frame* GetCurrentFrame() const;
        [[nodiscard]] const std::string& GetCurrentFrameName() const;
        [[nodiscard]] const unsigned int &GetCurrentFrameIndex() const;

        [[nodiscard]] bool ContainsFrame(const std::string& name) const;
        [[nodiscard]] bool ContainsFrame(unsigned int index) const;

        void SetFrame(const std::string& name);
        void SetFrame(unsigned int index);

    protected:
        void Update(const sf::Time& delta) override;
        Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

        void Invalidate() override;

    private:
        void UpdateVertices();
        void ApplyFrame(const Frame& frame);

        std::array<sf::Vertex, 4> m_vertices;
        const sf::Texture* m_texture;
        sf::IntRect m_texcoords;
        sf::FloatRect m_bounds;
        SizeMode m_sizeMode;
        Gx::BlendMode m_blendMode;

        std::string m_frameName;
        unsigned int m_frameIndex;
        const Frame* m_currentFrame;
        std::vector<std::string> m_indices;
        std::unordered_map<std::string, Frame> m_frames;
    };
}
