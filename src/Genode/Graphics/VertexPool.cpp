#include <Genode/Graphics/VertexPool.hpp>
#include <algorithm>
#include <cassert>

namespace Gx
{
    VertexPool::VertexPool(const sf::PrimitiveType primitiveType) :
        m_primitive(primitiveType)
    {
    }

    VertexPool::VertexPool(std::size_t capacity)
    {
        m_vertices.reserve(capacity);
        m_segments.emplace_back(0, capacity, false);
    }

    VertexPool::VertexPool(sf::PrimitiveType primitiveType, const std::size_t capacity) :
        VertexPool(capacity)
    {
        m_primitive = primitiveType;
    }

    VertexPool::Segment::Segment(const std::size_t offset, const std::size_t size, const bool inUse) :
        m_offset(offset),
        m_size(size),
        m_inUse(inUse)
    {
    }

    VertexSpan VertexPool::Rent(const std::size_t size)
    {
        if (size == 0)
        {
            return { *this, 0, size };
        }

        const auto segmentIdx = Scan(size);
        if (!segmentIdx)
        {
            std::size_t newOffset = m_vertices.size();
            m_vertices.resize(newOffset + size);
            m_segments.emplace_back(newOffset, size, true);

            return { *this, newOffset, size };
        }

        auto& segment = m_segments[*segmentIdx];
        if (segment.m_size > size)
        {
            m_segments.insert(
                m_segments.begin() + static_cast<VertexSpan::difference_type>(*segmentIdx) + 1,
                Segment(segment.m_offset + size, segment.m_size - size, false)
            );

            segment.m_size = size;
        }

        segment.m_inUse = true;
        return { *this, segment.m_offset, size };
    }

    void VertexPool::Return(VertexSpan& span)
    {
        std::size_t offset = span.offset();
        std::size_t size   = span.size();

        // Clear the vertices in the span and invalidate it
        std::fill(span.begin(), span.end(), sf::Vertex{ {}, sf::Color::Transparent, {} });

        // Find and mark the segment as unused
        const auto it = std::find_if(m_segments.begin(), m_segments.end(), [offset, size] (const Segment& s)
        {
            return s.m_offset == offset && s.m_size == size;
        });

        // Merge free segments into one
        if (it != m_segments.end())
        {
            it->m_inUse = false;
            Defragment();
        }
    }

    VertexSpan VertexPool::Transfer(const std::vector<sf::Vertex>& vertices)
    {
        if (vertices.empty())
        {
            return { *this, 0, 0 };
        }

        VertexSpan span = Rent(vertices.size());
        std::copy(vertices.begin(), vertices.end(), span.begin());
        return span;
    }

    VertexSpan VertexPool::Transfer(const sf::VertexArray& vertices)
    {
        const auto count = vertices.getVertexCount();
        if (count == 0)
        {
            return { *this, 0, 0 };
        }

        VertexSpan span = Rent(count);
        for (std::size_t i = 0; i < count; ++i)
        {
            span[i] = vertices[i];
        }

        return span;
    }

    void VertexPool::Reset()
    {
        std::fill(m_vertices.begin(), m_vertices.end(), sf::Vertex{ {}, sf::Color::Transparent, {} });
        m_segments.clear();
        m_segments.emplace_back(0, m_vertices.size(), false);
    }

    void VertexPool::Clear()
    {
        m_vertices.clear();
        m_segments.clear();
        m_segments.emplace_back(0, m_vertices.size(), false);
    }

    std::size_t VertexPool::GetCapacity() const noexcept
    {
        return m_vertices.capacity();
    }

    std::size_t VertexPool::GetSize() const noexcept
    {
        return m_vertices.size();
    }

    bool VertexPool::IsEmpty() const noexcept
    {
        return m_vertices.empty();
    }

    const std::vector<sf::Vertex>& VertexPool::GetVertices() const noexcept
    {
        return m_vertices;
    }

    const sf::Vertex* VertexPool::GetData() const noexcept
    {
        return m_vertices.data();
    }

    sf::Vertex* VertexPool::GetData() noexcept
    {
        return m_vertices.data();
    }

    RenderStates VertexPool::Render(RenderSurface& surface, RenderStates states) const
    {
        surface.Render(m_vertices.data(), m_vertices.size(), m_primitive, states);
        return states;
    }

    std::optional<std::size_t> VertexPool::Scan(const std::size_t size) const
    {
        for (std::size_t i = 0; i < m_segments.size(); ++i)
        {
            if (!m_segments[i].m_inUse && m_segments[i].m_size >= size)
            {
                return i;
            }
        }

        return std::nullopt;
    }

    void VertexPool::Defragment()
    {
        if (m_segments.empty())
        {
            return;
        }

        std::vector<Segment> newSegments;
        newSegments.push_back(m_segments[0]);

        for (std::size_t i = 1; i < m_segments.size(); ++i)
        {
            Segment& last = newSegments.back();
            Segment& current = m_segments[i];

            if (!last.m_inUse && !current.m_inUse &&
                last.m_offset + last.m_size == current.m_offset)
            {
                // Merge segments
                last.m_size += current.m_size;
            }
            else
            {
                newSegments.push_back(current);
            }
        }

        m_segments = std::move(newSegments);
    }
}

namespace Gx
{
    VertexSpan::VertexSpan(VertexPool& pool, const size_type offset, const size_type size) :
        m_pool(pool),
        m_offset(offset),
        m_size(size)
    {
    }

    VertexSpan& VertexSpan::operator=(VertexSpan&& other) noexcept
    {
        if (this == &other)
            return *this;

        m_pool   = other.m_pool;
        m_offset = other.m_offset;
        m_size   = other.m_size;

        return *this;
    }

    VertexSpan::reference VertexSpan::operator[](const size_type idx)
    {
        assert(idx < m_size);
        return m_pool.m_vertices[m_offset + idx];
    }

    VertexSpan::const_reference VertexSpan::operator[](const size_type idx) const
    {
        assert(idx < m_size);
        return m_pool.m_vertices[m_offset + idx];
    }

    VertexSpan::reference VertexSpan::front()
    {
        assert(!empty());
        return m_pool.m_vertices[m_offset];
    }

    VertexSpan::const_reference VertexSpan::front() const
    {
        assert(!empty());
        return m_pool.m_vertices[m_offset];
    }

    VertexSpan::reference VertexSpan::back()
    {
        assert(!empty());
        return m_pool.m_vertices[m_offset + m_size - 1];
    }

    VertexSpan::const_reference VertexSpan::back() const
    {
        assert(!empty());
        return m_pool.m_vertices[m_offset + m_size - 1];
    }

    VertexSpan::pointer VertexSpan::data()
    {
        return m_pool.m_vertices.data() + m_offset;
    }

    VertexSpan::const_pointer VertexSpan::data() const
    {
        return m_pool.m_vertices.data() + m_offset;
    }

    VertexSpan::iterator VertexSpan::begin() noexcept
    {
        return m_pool.m_vertices.begin() + static_cast<difference_type>(m_offset);
    }

    VertexSpan::const_iterator VertexSpan::begin() const noexcept
    {
        return m_pool.m_vertices.begin() + static_cast<difference_type>(m_offset);
    }

    VertexSpan::const_iterator VertexSpan::cbegin() const noexcept
    {
        return m_pool.m_vertices.cbegin() + static_cast<difference_type>(m_offset);
    }

    VertexSpan::iterator VertexSpan::end() noexcept
    {
        return m_pool.m_vertices.begin() + static_cast<difference_type>(m_offset) + static_cast<difference_type>(m_size);
    }

    VertexSpan::const_iterator VertexSpan::end() const noexcept
    {
        return m_pool.m_vertices.begin() + static_cast<difference_type>(m_offset) + static_cast<difference_type>(m_size);
    }

    VertexSpan::const_iterator VertexSpan::cend() const noexcept
    {
        return m_pool.m_vertices.cbegin() + static_cast<difference_type>(m_offset) + static_cast<difference_type>(m_size);
    }

    VertexSpan::reverse_iterator VertexSpan::rbegin() noexcept
    {
        return reverse_iterator(end());
    }

    VertexSpan::const_reverse_iterator VertexSpan::rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    VertexSpan::const_reverse_iterator VertexSpan::crbegin() const noexcept
    {
        return const_reverse_iterator(cend());
    }

    VertexSpan::reverse_iterator VertexSpan::rend() noexcept
    {
        return reverse_iterator(begin());
    }

    VertexSpan::const_reverse_iterator VertexSpan::rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    VertexSpan::const_reverse_iterator VertexSpan::crend() const noexcept
    {
        return const_reverse_iterator(cbegin());
    }

    bool VertexSpan::empty() const noexcept
    {
        return m_size == 0;
    }
    
    VertexSpan::size_type VertexSpan::size() const noexcept
    {
        return m_size;
    }

    VertexSpan::size_type VertexSpan::offset() const noexcept
    {
        return m_offset;
    }

    const std::vector<sf::Vertex>& VertexSpan::container() const noexcept
    {
        return m_pool.GetVertices();
    }

    bool VertexSpan::operator==(const VertexSpan& other) const noexcept
    {
        return &m_pool == &other.m_pool &&
               m_offset == other.m_offset &&
               m_size == other.m_size;
    }

    bool VertexSpan::operator!=(const VertexSpan& other) const noexcept
    {
        return !(*this == other);
    }
}
