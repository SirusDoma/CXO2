#pragma once

#include <Genode/Entities/Updatable.hpp>
#include <Genode/Graphics/VertexPool.hpp>

namespace Gx
{
    class Particle : public Updatable
    {
    public:
        explicit Particle(VertexSpan&& vertices) noexcept :
            m_vertices(std::move(vertices)),
            m_active{true}
        {}

        [[nodiscard]] bool IsActive() const { return m_active; }
        virtual void Destroy() { m_active = false; }

    protected:
        VertexSpan& GetVertices() { return m_vertices; }

    private:
        VertexSpan m_vertices;
        bool       m_active;
    };
}