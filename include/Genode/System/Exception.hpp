#ifndef GENODE_EXCEPTION_HPP
#define GENODE_EXCEPTION_HPP

#include <stdexcept>
#include <string>

namespace Gx
{
    class Exception : public std::runtime_error
    {
    public:
        explicit Exception(const std::string& message) : std::runtime_error(message) {};
    };

    class NotSupportedException : public Exception
    {
    public:
        explicit NotSupportedException(const std::string& message = "The operation is not supported.") : Exception(message) {};
    };
}

#endif