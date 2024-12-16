#pragma once

#include <Genode/Graphics/RenderStates.hpp>

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/VertexBuffer.hpp>
#include <SFML/Graphics/View.hpp>

namespace Gx
{
    class Renderable;
    class RenderSurface
    {
    public:
        virtual ~RenderSurface() = default;

        virtual void Clear() { Clear(sf::Color::Black); }
        virtual void Clear(sf::Color clearColor) = 0;
        virtual void Clear(sf::Color clearColor, sf::StencilValue stencilValue) = 0;

        virtual void Render(const Renderable& renderable, const RenderStates& states = RenderStates::Default) = 0;

        void Render(const sf::VertexArray& vertices, const RenderStates& states = RenderStates::Default)
        {
            if (vertices.getVertexCount() > 0)
                Render(&vertices[0], vertices.getVertexCount(), vertices.getPrimitiveType(), states);
        }

        virtual void Render(const sf::Vertex*       vertices,
                            std::size_t       vertexCount,
                            sf::PrimitiveType type,
                            const RenderStates&     states = RenderStates::Default
        ) = 0;

        virtual void Render(const sf::VertexBuffer& vertexBuffer, const RenderStates& states = RenderStates::Default) = 0;
        virtual void Render(const sf::VertexBuffer& vertexBuffer,
                            std::size_t       firstVertex,
                            std::size_t       vertexCount,
                            const RenderStates&     states = RenderStates::Default
        ) = 0;

        [[nodiscard]] virtual const sf::View& GetDefaultView() const = 0;
        [[nodiscard]] virtual const sf::View& GetView() const = 0;
        virtual void SetView(const sf::View& view) = 0;
    };
}
