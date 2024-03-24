#include <Genode/Graphics/RenderStates.hpp>

namespace Gx
{
    const RenderStates RenderStates::Default = RenderStates(sf::RenderStates::Default);

    RenderStates::RenderStates(const RenderStates &copy) :
        sf::RenderStates(copy),
        FrameID(copy.FrameID),
        Delta(copy.Delta)
    {

    }

    RenderStates::RenderStates(sf::RenderStates states, unsigned int frameID, double delta):
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


