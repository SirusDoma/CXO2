#ifndef GENODE_LABEL_HPP
#define GENODE_LABEL_HPP

#include <Genode/UI/Control.hpp>
#include <Genode/Graphics/Text.hpp>

namespace Gx
{
    class Label : public virtual Control, public virtual Text
    {
    public:
        using Text::Text;
        virtual ~Label();

        virtual const sf::FloatRect GetLocalBounds() const;

    protected:
        virtual void Update(double delta);
        virtual sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const;

        virtual void Invalidate();
    };
}

#endif
