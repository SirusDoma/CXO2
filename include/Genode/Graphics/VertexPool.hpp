#pragma once

#include <Genode/Entities/Poolable.hpp>

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include <vector>
#include <optional>
#include <iterator>

namespace Gx
{
    class VertexSpan final
    {
    public:
        using value_type             = sf::Vertex;
        using size_type              = std::size_t;
        using difference_type        = std::ptrdiff_t;
        using reference              = value_type&;
        using const_reference        = const value_type&;
        using pointer                = value_type*;
        using const_pointer          = const value_type*;
        using iterator               = std::vector<sf::Vertex>::iterator;
        using const_iterator         = std::vector<sf::Vertex>::const_iterator;
        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        reference operator[](size_type idx);
        const_reference operator[](size_type idx) const;
        reference front();
        [[nodiscard]] const_reference front() const;
        reference back();
        [[nodiscard]] const_reference back() const;
        pointer data();
        [[nodiscard]] const_pointer data() const;

        iterator begin() noexcept;
        [[nodiscard]] const_iterator begin() const noexcept;
        [[nodiscard]] const_iterator cbegin() const noexcept;
        iterator end() noexcept;
        [[nodiscard]] const_iterator end() const noexcept;
        [[nodiscard]] const_iterator cend() const noexcept;
        reverse_iterator rbegin() noexcept;
        [[nodiscard]] const_reverse_iterator rbegin() const noexcept;
        [[nodiscard]] const_reverse_iterator crbegin() const noexcept;
        reverse_iterator rend() noexcept;
        [[nodiscard]] const_reverse_iterator rend() const noexcept;
        [[nodiscard]] const_reverse_iterator crend() const noexcept;

        [[nodiscard]] bool empty() const noexcept;
        [[nodiscard]] size_type size() const noexcept;
        [[nodiscard]] size_type offset() const noexcept;

        [[nodiscard]] const std::vector<sf::Vertex>& container() const noexcept;

        bool operator==(const VertexSpan& other) const noexcept;
        bool operator!=(const VertexSpan& other) const noexcept;

    private:
        friend class VertexPool;

        VertexSpan(VertexPool& pool, size_type offset, size_type size);

        VertexPool& m_pool;
        size_type   m_offset = {};
        size_type   m_size   = {};
    };

    class VertexPool : public Poolable<VertexSpan>
    {
    public:
        VertexPool() = default;
        explicit VertexPool(std::size_t capacity);

        [[nodiscard]] VertexSpan Rent(std::size_t size) override;
        void Return(VertexSpan& span) override;

        [[nodiscard]] VertexSpan Transfer(const std::vector<sf::Vertex>& vertices);
        [[nodiscard]] VertexSpan Transfer(const sf::VertexArray& vertices);

        void Reset();
        void Clear();

        [[nodiscard]] std::size_t GetCapacity() const noexcept;
        [[nodiscard]] std::size_t GetSize() const noexcept;
        [[nodiscard]] bool IsEmpty() const noexcept;

        [[nodiscard]] const std::vector<sf::Vertex>& GetVertices() const noexcept;
        [[nodiscard]] const sf::Vertex* GetData() const noexcept;
        [[nodiscard]] sf::Vertex* GetData() noexcept;

    private:
        friend class VertexSpan;

        struct Segment
        {
            std::size_t m_offset = {};
            std::size_t m_size   = {};
            bool m_inUse         = {};

            Segment(std::size_t offset, std::size_t size, bool inUse = false);
        };

        [[nodiscard]] std::optional<std::size_t> Scan(std::size_t size) const;
        void Defragment();

        std::vector<sf::Vertex> m_vertices = {};
        std::vector<Segment> m_segments    = {};
    };
}
