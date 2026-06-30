#pragma once

#include <Genode/Graphics/Transformable.hpp>
#include <Genode/Graphics/Text.hpp>

#include <memory>
#include <string>

namespace Cx
{
    class Console : public Gx::Transformable, public Gx::Renderable
    {
    public:
        static Console& Instance();
        Console() = default;

        bool IsEnabled() const;
        void SetEnabled(const bool enabled);

        const Gx::Font* GetFont() const;
        void SetFont(const Gx::Font& font);

        unsigned int GetCharacterSize() const;
        void SetCharacterSize(unsigned int size);

        float GetLineSpacing() const;
        void SetLineSpacing(float spacing);

        unsigned int GetMaximumLines() const;
        void SetMaximumLines(unsigned int max);

        sf::FloatRect GetBounds() const;
        void SetBounds(const sf::FloatRect& bounds);

        void Write(const std::string& message);

    private:
        void Invalidate();
        Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

        inline static std::unique_ptr<Console> m_instance;

        Gx::Text m_text;
        unsigned int m_maxLines = 10;
        sf::FloatRect m_bounds;
        std::vector<std::string> m_logs;
        bool m_enabled;
    };
}
