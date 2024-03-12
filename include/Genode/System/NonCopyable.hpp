#ifndef GENODE_NONCOPYABLE_HPP
#define GENODE_NONCOPYABLE_HPP

namespace Gx
{
    class NonCopyable
    {
    public:
        NonCopyable() = default;
        NonCopyable(const NonCopyable&) = delete;
        NonCopyable& operator=(const NonCopyable&) = delete;
    };
}

#endif