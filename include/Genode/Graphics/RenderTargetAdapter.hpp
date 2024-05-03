#ifndef GENODE_RENDER_TARGET_ADAPTER_HPP
#define GENODE_RENDER_TARGET_ADAPTER_HPP

#include <Genode/Graphics/RenderSurface.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

namespace Gx
{
    class RenderTargetAdapter : public RenderSurface
    {
    public:
        explicit RenderTargetAdapter(sf::RenderTarget &target);
        ~RenderTargetAdapter() override = default;

        void Render(const Renderable& renderable, const RenderStates& states = RenderStates::Default) override;
        void Render(const sf::Vertex*       vertices,
                    const std::size_t       vertexCount,
                    const sf::PrimitiveType type,
                    const RenderStates&     states = RenderStates::Default
        ) override;

        void Render(const sf::VertexBuffer& vertexBuffer, const RenderStates& states = RenderStates::Default) override;
        void Render(const sf::VertexBuffer& vertexBuffer,
                    const std::size_t       firstVertex,
                    const std::size_t       vertexCount,
                    const RenderStates&     states = RenderStates::Default
        ) override;

    private:
        sf::RenderTarget *m_target = nullptr;
    };
}

#endif
