#ifndef GENODE_IO_JSON_HPP
#define GENODE_IO_JSON_HPP

#include <string>
#include <nlohmann/json.hpp>

namespace Gx
{
    using Json = nlohmann::ordered_json;
}

#endif