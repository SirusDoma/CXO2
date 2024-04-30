#include <Genode/Graphics/RenderStates.hpp>
#include <Genode/Graphics/SpriteBatch.hpp>

namespace Gx
{
    const RenderStates RenderStates::Default(sf::RenderStates(sf::BlendMode(
            sf::BlendMode::Factor::SrcAlpha,
            sf::BlendMode::Factor::OneMinusSrcAlpha,
            sf::BlendMode::Equation::Add,
            sf::BlendMode::Factor::One,
            sf::BlendMode::Factor::OneMinusSrcAlpha,
            sf::BlendMode::Equation::Add)));

    RenderStates::RenderStates(const sf::RenderStates &states, const unsigned int frameID, const double delta) :
        sf::RenderStates(states),
        FrameID(frameID),
        Delta(delta)
    {
    }

    RenderStates &RenderStates::operator=(const RenderStates &states)
    {
        sf::RenderStates::operator=(states);
        return *this;
    }

}


