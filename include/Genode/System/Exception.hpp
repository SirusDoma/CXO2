#pragma once

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
        explicit NotSupportedException(const std::string& message = "Specified method is not supported.") : Exception(message) {};
    };

    class InvalidOperationException : public Exception
    {
    public:
        explicit InvalidOperationException(const std::string& message = "Operation is not valid due to the current state of the object.") : Exception(message) {};
    };

    class ArgumentException  : public Exception
    {
    public:
        explicit ArgumentException(const std::string& message = "Value does not fall within the expected range.") : Exception(message) {};
        explicit ArgumentException(const std::string& parameter, const std::string& message ) : Exception("\"" + parameter +"\": " + message), m_parameter(parameter) {};

        const std::string& GetParameterName() const { return m_parameter; }

    private:
        std::string m_parameter;
    };

    class ArgumentOutOfRangeException  : public Exception
    {
    public:
        explicit ArgumentOutOfRangeException(const std::string& message = "Specified argument was out of the range of valid values.") : Exception(message) {};
        explicit ArgumentOutOfRangeException(const std::string& parameter, const std::string& message) : Exception("\"" + parameter +"\": " + message), m_parameter(parameter) {};

        const std::string& GetParameterName() const { return m_parameter; }

    private:
        std::string m_parameter;
    };
}
