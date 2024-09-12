////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2022 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <Genode/Graphics/SpriteBatch.hpp>
#include <Genode/System/Exception.hpp>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>
#include <numeric>
namespace Gx
{
    SpriteBatch::SpriteBatch(const BatchMode batchMode) :
        m_batchMode(batchMode)
    {
    }

    ////////////////////////////////////////////////////////////
    void SpriteBatch::SetBatchMode(const BatchMode batchMode)
    {
        m_batchMode      = batchMode;
        m_updateRequired = true;
    }

    ////////////////////////////////////////////////////////////
    void SpriteBatch::Batch(const sf::VertexArray& vertices, const sf::Texture* texture, const sf::Transform& transform, const float layer)
    {
        Batch(&vertices[0], vertices.getVertexCount(), vertices.getPrimitiveType(), texture, transform, layer);
    }

    ////////////////////////////////////////////////////////////
    void SpriteBatch::Batch(const sf::Vertex*       vertices,
                            const std::size_t       count,
                            const sf::PrimitiveType type,
                            const sf::Texture*      texture,
                            const sf::Transform&    transform,
                            const float             layer)
    {
        if (type != sf::PrimitiveType::TriangleFan && type != sf::PrimitiveType::TriangleStrip && type != sf::PrimitiveType::Triangles)
           throw Exception("SpriteBatch supports only triangle-based primitive types");

        // Anything to batch?
        if (count == 0)
            return;

        // Split into triangles
        switch (type)
        {
            case sf::PrimitiveType::TriangleStrip:
                for (std::size_t i = 2; i < count; i++)
                    PushTriangle(vertices[i - 2], vertices[i - 1], vertices[i], transform, texture, layer);

                break;
            case sf::PrimitiveType::TriangleFan:
                for (std::size_t i = 2; i < count; i++)
                    PushTriangle(vertices[0], vertices[i - 1], vertices[i], transform, texture, layer);

                break;
            case sf::PrimitiveType::Triangles:
                for (std::size_t i = 2; i < count; i += 3)
                    PushTriangle(vertices[i - 2], vertices[i - 1], vertices[i], transform, texture, layer);

                break;
            default:
                throw Exception("A non-triangle primitive type was encountered when decomposing into triangles");
        }
    }

    ////////////////////////////////////////////////////////////
    void SpriteBatch::Update(const double delta)
    {
        UpdatableContainer::Update(delta);
    }

    ////////////////////////////////////////////////////////////
    RenderStates SpriteBatch::Render(RenderSurface& surface, RenderStates states) const
    {
        if (!IsVisible())
            return states;

        // Apply render states
        states.transform *= GetTransform();
        states.blendMode  = m_blendMode.value_or(states.blendMode);

        // Calculate and populate the vertices to render
        auto& batcher = const_cast<SpriteBatch&>(*this);
        RenderableContainer::Render(batcher, states);
        UpdateBatch();

        // Render the batches
        auto cstates = states;
        std::size_t startTriangle = 0;
        for (const auto& batch : m_batches)
        {
            cstates.texture = batch.texture;
            surface.Render(&m_vertices[startTriangle], batch.vertexCount, sf::PrimitiveType::Triangles, cstates);

            startTriangle += batch.vertexCount;
        }

        // Signal clear is required in next render/update
        batcher.ClearBatch(true); // HACK: Call non-const in Render
        return states;
    }

    void SpriteBatch::Clear(const sf::Color)
    {
        ClearBatch();
    }

    void SpriteBatch::Clear(const sf::Color, sf::StencilValue)
    {
        ClearBatch();
    }

    ////////////////////////////////////////////////////////////
    void SpriteBatch::Render(const Renderable& renderable, const RenderStates& states)
    {
        renderable.Render(*this, states);
    }

    ////////////////////////////////////////////////////////////
    void SpriteBatch::Render(const sf::Vertex* vertices, const std::size_t vertexCount, const sf::PrimitiveType type, const RenderStates& states)
    {
        if (m_blendMode.has_value() && m_blendMode != states.blendMode)
            throw NotSupportedException("Multiple blending mode usage within single batch is not supported");

        ClearBatch();

        m_blendMode = states.blendMode;
        Batch(vertices, vertexCount, type, states.texture, states.transform, states.Layer);
    }

    ////////////////////////////////////////////////////////////
    void SpriteBatch::Render(const sf::VertexBuffer& vertexBuffer, const RenderStates& states)
    {
        throw NotSupportedException("Vertex Buffer is already live in GPU Memory");
    }

    ////////////////////////////////////////////////////////////
    void SpriteBatch::Render(const sf::VertexBuffer& vertexBuffer, const std::size_t firstVertex, const std::size_t vertexCount, const RenderStates& states)
    {
        throw NotSupportedException("Vertex Buffer is already live in GPU Memory");
    }

    const sf::View& SpriteBatch::GetDefaultView() const
    {
        throw NotSupportedException("SpriteBatch does not have a view");
    }

    const sf::View& SpriteBatch::GetView() const
    {
        throw NotSupportedException("SpriteBatch does not have a view");
    }

    void SpriteBatch::SetView(const sf::View& view)
    {
        throw NotSupportedException("SpriteBatch does not have a view");
    }

    ////////////////////////////////////////////////////////////
    void SpriteBatch::PushTriangle(const sf::Vertex&    a,
                                   const sf::Vertex&    b,
                                   const sf::Vertex&    c,
                                   const sf::Transform& transform,
                                   const sf::Texture*   texture,
                                   float                level)
    {
        m_triangles.emplace_back(texture, level);

        m_unsortedVertices.push_back({transform * a.position, a.color, a.texCoords});
        m_unsortedVertices.push_back({transform * b.position, b.color, b.texCoords});
        m_unsortedVertices.push_back({transform * c.position, c.color, c.texCoords});

        m_updateRequired = true;
    }

    ////////////////////////////////////////////////////////////
    void SpriteBatch::UpdateBatch(const bool force) const
    {
        if (!force && !m_updateRequired)
            return;

        if (m_batchMode == BatchMode::Deferred)
        {
            // Batch based on sf::Texture change
            std::size_t    startIndex  = 0;
            std::size_t    nextIndex   = 0;
            const sf::Texture* lastTexture = nullptr;

            while (nextIndex < m_triangles.size())
            {
                if (const sf::Texture* nextTexture = m_triangles[nextIndex].texture; nextTexture != lastTexture)
                {
                    m_batches.emplace_back(lastTexture, (nextIndex - startIndex) * 3);
                    lastTexture = nextTexture;
                    startIndex  = nextIndex;
                }

                nextIndex++;
            }

            // Deal with leftovers
            if (startIndex != m_triangles.size())
                m_batches.emplace_back(lastTexture, (m_triangles.size() - startIndex) * 3);

            m_vertices = m_unsortedVertices;
        }
        else if (m_batchMode == BatchMode::TextureSort || m_batchMode == BatchMode::LayerSort)
        {
            std::vector<std::size_t> indices(m_triangles.size());

            std::iota(indices.begin(), indices.end(), 0);

            if (m_batchMode == BatchMode::TextureSort)
            {
                const auto comp = [this](const std::size_t a, const std::size_t b)
                { return m_triangles[a].texture < m_triangles[b].texture; };

                std::stable_sort(indices.begin(), indices.end(), comp);
            }
            else
            {
                const auto comp = [this](const std::size_t a, const std::size_t b)
                {
                    if (m_triangles[a].level != m_triangles[b].level)
                        return m_triangles[a].level < m_triangles[b].level;
                    else
                        return m_triangles[a].texture < m_triangles[b].texture;
                };

                std::stable_sort(indices.begin(), indices.end(), comp);
            }

            // Create the array of sorted vertices
            // We need them sorted so that we feed chunks to RenderTarget
            m_vertices.resize(m_unsortedVertices.size());

            for (std::size_t i = 0; i < indices.size(); i++)
            {
                const std::size_t newPos = i * 3;
                const std::size_t oldPos = indices[i] * 3;
                m_vertices[newPos]       = m_unsortedVertices[oldPos];
                m_vertices[newPos + 1]   = m_unsortedVertices[oldPos + 1];
                m_vertices[newPos + 2]   = m_unsortedVertices[oldPos + 2];
            }

            // Generate batches
            std::size_t    startIndex  = 0;
            std::size_t    nextIndex   = 0;
            const sf::Texture* lastTexture = nullptr;

            while (nextIndex < m_triangles.size())
            {
                const sf::Texture* nextTexture = m_triangles[indices[nextIndex]].texture;

                if (nextTexture != lastTexture)
                {
                    m_batches.emplace_back(lastTexture, (nextIndex - startIndex) * 3);
                    lastTexture = nextTexture;
                    startIndex  = nextIndex;
                }

                nextIndex++;
            }

            // Deal with leftovers
            if (startIndex != m_triangles.size())
                m_batches.emplace_back(m_triangles[indices[startIndex]].texture, (m_triangles.size() - startIndex) * 3);
        }
    }


    ////////////////////////////////////////////////////////////
    void SpriteBatch::ClearBatch(const bool force)
    {
        if (!force && !m_clearRequired)
            return;

        m_unsortedVertices.clear();
        m_triangles.clear();
        m_vertices.clear();
        m_batches.clear();

        m_updateRequired = false;
    }

} // namespace Gx
