#pragma once

#include <cstddef>

namespace Gx
{
    template<typename TSpan>
    class Poolable
    {
    public:
        virtual ~Poolable() = default;

        [[nodiscard]] virtual TSpan Rent(std::size_t count) = 0;
        virtual void Return(TSpan& span) = 0;
    };
}