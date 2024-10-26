#pragma once

#include <Genode/UI/Control.hpp>
#include <Genode/Graphics/Text.hpp>

namespace Gx
{
    class Label : public virtual Control, public virtual Text
    {
    public:
        enum class Alignment { None, Left, Center, Right };

        using Text::Text;

        sf::FloatRect GetLocalBounds() const override;

        Alignment GetAlignment() const;
        void SetAlignment(Alignment alignment);

    protected:
        void Update(double delta) override;
        RenderStates Render(RenderSurface& surface, RenderStates states) const override;

        void OnGeometryUpdated() const override;
        void Invalidate() override;

    private:
        mutable bool m_alignmentUpdated = false;
        Alignment m_alignment = Alignment::None;
    };
}
