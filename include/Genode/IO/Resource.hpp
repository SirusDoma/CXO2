#pragma once

#include <functional>
#include <memory>

namespace Gx
{
    template<typename R>
    using ResourceDeleter = std::function<void(R*)>;

    template<typename R>
    using ResourcePtr = std::unique_ptr<R, ResourceDeleter<R>>;

    template<typename T, typename V>
    std::enable_if_t<std::is_base_of_v<T, V>, ResourcePtr<T>>
    Cast(ResourcePtr<V>&& target)
    {
        return ResourcePtr<T>(target.release(), [deleter = target.get_deleter()] (T* ptr)
        {
            deleter(dynamic_cast<V*>(ptr));
        });
    }

    template<typename T, typename V>
    std::enable_if_t<std::is_base_of_v<V, T>, ResourcePtr<T>>
    Cast(ResourcePtr<V>&& target)
    {
        if (auto source = dynamic_cast<T*>(target.get()))
        {
            target.release();
            return ResourcePtr<T>(source, [deleter = target.get_deleter()] (T* ptr)
            {
                deleter(ptr);
            });
        }

        return nullptr;
    }
}
