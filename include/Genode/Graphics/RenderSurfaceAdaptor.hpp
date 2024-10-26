#pragma once

#include <Genode/Graphics/RenderSurface.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <functional>

namespace Gx
{
    class RenderSurfaceAdaptor : public virtual RenderSurface
    {
    public:
        explicit RenderSurfaceAdaptor(sf::RenderTarget& target);

        void SetClearColorResolver(const std::function<sf::Color()>& resolver);

        void Clear() override;
        void Clear(const sf::Color clearColor) override;
        void Clear(const sf::Color clearColor, sf::StencilValue stencilValue) override;

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

        const sf::View& GetDefaultView() const override;
        const sf::View& GetView() const override;
        void SetView(const sf::View& view) override;

    private:
        sf::RenderTarget* m_target = nullptr;
        std::function<sf::Color()> m_clearColorResolver;
    };
}
