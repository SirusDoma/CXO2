#ifndef GENODE_UTILITIES_RESOURCE_CASTER_HPP
#define GENODE_UTILITIES_RESOURCE_CASTER_HPP

#include <Genode/IO/Resource.hpp>

namespace Gx
{
    template<typename TR, class SR>
    inline static ResourcePtr<TR> ResourceCast(ResourcePtr<SR> resource)
    {
        //static_assert(std::is_base_of_v<TR, SR>, "resource classes are unrelated.");

        auto deleter  = resource.get_deleter();
        auto entity   = resource.release();

        return ResourcePtr<TR>{dynamic_cast<TR*>(entity), [deleter] (auto ptr) { deleter(dynamic_cast<SR*>(ptr)); }};
    }
}

#endif
