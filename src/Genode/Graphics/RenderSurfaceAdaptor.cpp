#include <Genode/Graphics/RenderSurfaceAdaptor.hpp>
#include <Genode/Entities/Renderable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

Gx::RenderSurfaceAdaptor::RenderSurfaceAdaptor(sf::RenderTarget& target) :
    m_target(&target),
    m_clearColorResolver()
{
}

void Gx::RenderSurfaceAdaptor::SetClearColorResolver(const std::function<sf::Color()>& resolver)
{
    m_clearColorResolver = resolver;
}

void Gx::RenderSurfaceAdaptor::Clear()
{
    if (m_clearColorResolver)
        Clear(m_clearColorResolver());
    else
        RenderSurface::Clear();
}

void Gx::RenderSurfaceAdaptor::Clear(const sf::Color clearColor)
{
    m_target->clear(clearColor);
}

void Gx::RenderSurfaceAdaptor::Clear(const sf::Color clearColor, const sf::StencilValue stencilValue)
{
    m_target->clear(clearColor, stencilValue);
}

void Gx::RenderSurfaceAdaptor::Render(const Renderable& renderable, const RenderStates& states)
{
    renderable.Render(*this, states);
}

void Gx::RenderSurfaceAdaptor::Render(const sf::Vertex* vertices, const std::size_t vertexCount, const sf::PrimitiveType type, const RenderStates& states)
{
    m_target->draw(vertices, vertexCount, type, states);
}

void Gx::RenderSurfaceAdaptor::Render(const sf::VertexBuffer& vertexBuffer, const RenderStates& states)
{
    m_target->draw(vertexBuffer, states);
}

void Gx::RenderSurfaceAdaptor::Render(const sf::VertexBuffer& vertexBuffer, const std::size_t firstVertex, const std::size_t vertexCount, const RenderStates& states)
{
    m_target->draw(vertexBuffer, firstVertex, vertexCount, states);
}

const sf::View& Gx::RenderSurfaceAdaptor::GetDefaultView() const
{
    return m_target->getDefaultView();
}

const sf::View& Gx::RenderSurfaceAdaptor::GetView() const
{
    return m_target->getView();
}

void Gx::RenderSurfaceAdaptor::SetView(const sf::View& view)
{
    m_target->setView(view);
}
