#ifndef GENODE_SYSTEM_CONFIG_HPP
#define GENODE_SYSTEM_CONFIG_HPP

namespace Gx
{
    struct Config
    {
        Config() {};
        virtual ~Config() {};

        virtual void Reset() = 0;
    };
}

#endif
