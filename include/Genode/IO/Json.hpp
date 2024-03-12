#ifndef GENODE_IO_JSON_HPP
#define GENODE_IO_JSON_HPP

#include <string>
#include <nlohmann/json.hpp>

namespace Gx
{
    typedef nlohmann::ordered_json Json;
}

#endif