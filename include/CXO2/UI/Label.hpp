#pragma once

#include <CXO2/UI/Control.hpp>
#include <Genode/Graphics/Text.hpp>

#include <unordered_map>

namespace Cx
{
    class Label : public virtual Control, public virtual Gx::Text
    {
    public:
        enum class VerticalAlignment { Top, Center, Bottom };

        using Gx::Text::Text;

        void AddFallbackFont(const Gx::Font& font) const;

        void SetString(const sf::String& string);

        [[nodiscard]] VerticalAlignment GetVerticalAlignment() const;
        void SetVerticalAlignment(VerticalAlignment alignment);

        [[nodiscard]] const sf::String& GetEllipsis() const;
        void SetEllipsis(const sf::String& ellipsis);

        [[nodiscard]] sf::FloatRect GetLocalBounds() const override;
        virtual void SetLocalBounds(const sf::FloatRect& bounds);

        void ClipQuads(const sf::FloatRect& rect) const;

    protected:
        void Update(const sf::Time& delta) override;
        Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

        void OnFontChanged(const Gx::Font&) const override;
        void OnGeometryUpdating() const override;
        void OnGeometryUpdated() const override;

        void Invalidate() override;

    private:
        void EnsureLayout() const;
        void Layout() const;
        void SetDisplayString(const sf::String& string) const;

        mutable const Gx::Font* m_defaultFont{nullptr};
        mutable std::unordered_set<const Gx::Font*> m_fallbackFonts{};

        sf::FloatRect m_bounds{};
        sf::String m_ellipsis{};
        VerticalAlignment m_verticalAlignment{VerticalAlignment::Center};
        mutable sf::String m_sourceString{};
        mutable sf::Vector2f m_alignOffset{};
        mutable bool m_hasSource{};
        mutable bool m_layoutNeeded{};
    };
}
