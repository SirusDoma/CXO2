#ifndef GENODE_SYSTEM_CONFIG_HPP
#define GENODE_SYSTEM_CONFIG_HPP

namespace Gx
{
    struct Config
    {
        Config() = default;
        virtual ~Config() = default;

        virtual void Reset() {};
    };
}

#endif
