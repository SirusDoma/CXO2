#include <cstddef>
#include <array>
#include <type_traits>
#include <iterator>

namespace Gx
{
    template<typename T>
    class Span
    {
    public:
        // Types
        using element_type = T;
        using value_type = std::remove_cv_t<T>;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        using iterator = pointer;
        using const_iterator = const_pointer;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        // Constructors
        constexpr Span() noexcept :
            m_data(nullptr),
            m_size(0)
        {
        }

        constexpr Span(pointer ptr, size_type count) noexcept :
            m_data(ptr),
            m_size(count)
        {
        }

        constexpr Span(pointer first, pointer last) noexcept :
            m_data(first),
            m_size(static_cast<size_type>(last - first))
        {
        }

        // Constructor from container (works with vector, array, etc.)
        template<typename Container>
        constexpr explicit Span(Container& cont,
            std::enable_if_t<
                std::is_convertible_v<
                    std::remove_pointer_t<decltype(std::data(cont))>(*)[],
                    T(*)[]
                >,
            int> = 0) noexcept
            : m_data(std::data(cont)),
              m_size(std::size(cont))
        {
        }

        // Constructor for arrays
        template<size_t N>
        constexpr explicit Span(element_type (&arr)[N]) noexcept :
            m_data(arr),
            m_size(N)
        {
        }

        template<size_t N>
        constexpr explicit Span(std::array<value_type, N>& arr) noexcept :
            m_data(arr.data()),
            m_size(N)
        {
        }

        template<size_t N>
        constexpr explicit Span(const std::array<value_type, N>& arr) noexcept :
            m_data(arr.data()),
            m_size(N)
        {
        }

        // Iterator support
        constexpr iterator begin() const noexcept
        {
            return m_data;
        }

        constexpr iterator end() const noexcept
        {
            return m_data + m_size;
        }

        constexpr const_iterator cbegin() const noexcept
        {
            return m_data;
        }

        constexpr const_iterator cend() const noexcept
        {
            return m_data + m_size;
        }

        constexpr reverse_iterator rbegin() const noexcept
        {
            return reverse_iterator(end());
        }

        constexpr reverse_iterator rend() const noexcept
        {
            return reverse_iterator(begin());
        }

        constexpr const_reverse_iterator crbegin() const noexcept
        {
            return const_reverse_iterator(cend());
        }

        constexpr const_reverse_iterator crend() const noexcept
        {
            return const_reverse_iterator(cbegin());
        }

        // Element access
        constexpr reference operator[](size_type idx) const noexcept
        {
            return m_data[idx];
        }

        constexpr reference front() const noexcept
        {
            return m_data[0];
        }

        constexpr reference back() const noexcept
        {
            return m_data[m_size - 1];
        }

        constexpr pointer data() const noexcept
        {
            return m_data;
        }

        // Observers
        constexpr size_type size() const noexcept
        {
            return m_size;
        }

        constexpr size_type size_bytes() const noexcept
        {
            return m_size * sizeof(element_type);
        }

        [[nodiscard]] constexpr bool empty() const noexcept
        {
            return m_size == 0;
        }

        // Subspan operations
        constexpr Span<element_type> first(size_type count) const
        {
            return Span<element_type>(m_data, count);
        }

        constexpr Span<element_type> last(size_type count) const
        {
            return Span<element_type>(m_data + (m_size - count), count);
        }

        constexpr Span<element_type> subspan(
            size_type offset,
            size_type count = size_type(-1)) const
        {
            if (count == size_type(-1))
            {
                count = m_size - offset;
            }
            return Span<element_type>(m_data + offset, count);
        }

    private:
        pointer m_data;
        size_type m_size;
    };

    // Deduction guides
    template<typename T, size_t N>
    Span(T (&)[N]) -> Span<T>;

    template<typename T, size_t N>
    Span(std::array<T, N>&) -> Span<T>;

} // namespace Gx
