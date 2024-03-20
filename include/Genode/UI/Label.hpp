#ifndef GENODE_UI_LABEL_HPP
#define GENODE_UI_LABEL_HPP

#include <Genode/UI/Control.hpp>
#include <Genode/Graphics/Text.hpp>

namespace Gx
{
    class Label : public virtual Control, public virtual Text
    {
    public:
        using Text::Text;
        ~Label() override = default;

        sf::FloatRect GetLocalBounds() const override;

    protected:
        void Update(double delta) override;
        sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const override;

        void Invalidate() override;
    };
}

#endif
