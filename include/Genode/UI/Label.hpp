#pragma once

#include <Genode/UI/Control.hpp>
#include <Genode/Graphics/Text.hpp>

#include <unordered_map>

namespace Gx
{
    class Label : public virtual Control, public virtual Text
    {
    public:
        enum class Alignment { None, Left, Center, Right };

        using Text::Text;

        void AddFallbackFont(const Font& font) const;

        sf::FloatRect GetLocalBounds() const override;
        virtual void SetLocalBounds(const sf::FloatRect& bounds);

        Alignment GetAlignment() const;
        void SetAlignment(Alignment alignment);

    protected:
        void Update(double delta) override;
        RenderStates Render(RenderSurface& surface, RenderStates states) const override;

        void OnFontChanged(const Gx::Font&) const override;
        void OnGeometryUpdating() const override;
        void OnGeometryUpdated() const override;

        void Invalidate() override;

    private:
        mutable bool m_alignmentUpdated = false;
        mutable const Font* m_defaultFont{nullptr};
        mutable std::unordered_set<const Font*> m_fallbackFonts{};

        Alignment m_alignment = Alignment::None;
        sf::FloatRect m_bounds{};
    };
}
