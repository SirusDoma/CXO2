#include <Genode/Graphics/RenderTargetAdapter.hpp>
#include <Genode/Entities/Renderable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

Gx::RenderTargetAdapter::RenderTargetAdapter(sf::RenderTarget &target)
    : m_target(&target)
{
}

void Gx::RenderTargetAdapter::Render(const Renderable& renderable, const RenderStates &states)
{
    m_target->draw(renderable, states);
}

void Gx::RenderTargetAdapter::Render(const sf::Vertex *vertices, const std::size_t vertexCount, const sf::PrimitiveType type, const RenderStates &states)
{
    m_target->draw(vertices, vertexCount, type, states);
}

void Gx::RenderTargetAdapter::Render(const sf::VertexBuffer &vertexBuffer, const RenderStates &states)
{
    m_target->draw(vertexBuffer, states);
}

void Gx::RenderTargetAdapter::Render(const sf::VertexBuffer &vertexBuffer, const std::size_t firstVertex, const std::size_t vertexCount, const RenderStates &states)
{
    m_target->draw(vertexBuffer, firstVertex, vertexCount, states);
}

const sf::View & Gx::RenderTargetAdapter::GetDefaultView() const
{
    return m_target->getDefaultView();
}

const sf::View& Gx::RenderTargetAdapter::GetView() const
{
    return m_target->getView();
}

void Gx::RenderTargetAdapter::SetView(const sf::View &view)
{
    m_target->setView(view);
}
