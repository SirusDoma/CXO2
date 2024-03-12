#ifndef GENODE_IO_RESOURCE_HPP
#define GENODE_IO_RESOURCE_HPP

#include <functional>
#include <memory>

namespace Gx
{
    template<typename R>
    using ResourceDeleter = std::function<void(R*)>;

    template<typename R>
    using ResourcePtr = std::unique_ptr<R, ResourceDeleter<R>>;

}

#endif
