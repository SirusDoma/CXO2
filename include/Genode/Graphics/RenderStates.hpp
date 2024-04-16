#ifndef GENODE_GRAPHICS_RENDER_STATES
#define GENODE_GRAPHICS_RENDER_STATES

#include <SFML/Graphics/RenderStates.hpp>

namespace Gx
{
    class RenderStates : public sf::RenderStates
    {
    public:
        using sf::RenderStates::RenderStates;

        explicit RenderStates(const sf::RenderStates &states, unsigned int frameID = 0, double delta = 0);

        RenderStates& operator=(const RenderStates& states);

        const unsigned int FrameID = 0;
        const double Delta = 0;

        static const RenderStates Default;
    };
}

#endif