#pragma once

#include <functional>
#include <memory>

namespace Gx
{
    template<typename R>
    using ResourceDeleter = std::function<void(R*)>;

    template<typename R>
    using ResourcePtr = std::unique_ptr<R, ResourceDeleter<R>>;

}
