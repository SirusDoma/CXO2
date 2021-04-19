#ifndef GENODE_SYSTEM_MODULE_HPP
#define GENODE_SYSTEM_MODULE_HPP

namespace Gx
{
    class Module
    {
    public:
        Module() {};
        virtual ~Module() = 0;
    };

    inline Module::~Module() {};
}

#endif