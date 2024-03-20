#include <Genode/UI/Label.hpp>

namespace Gx
{
    sf::FloatRect Label::GetLocalBounds() const
    {
        return Text::GetLocalBounds();
    }

    void Label::Update(double delta)
    {
        Control::Update(delta);
    }

    sf::RenderStates Label::Render(sf::RenderTarget &target, sf::RenderStates states) const
    {
        if (!IsVislble())
            return states;

        return Text::Render(target, states);
    }

    void Label::Invalidate()
    {
    }
}
