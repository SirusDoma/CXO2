#ifndef GENODE_RESOURCE_METADATA_HPP
#define GENODE_RESOURCE_METADATA_HPP

#include <string>
#include <vector>
#include <unordered_map>

namespace Gx
{
    struct ResourceMetadata
    {
    public:
        virtual ~ResourceMetadata() {}
        
        std::string Type;
        std::string Name;
        std::unordered_map<std::string, std::string> ResourceReferences;
    };
}

#endif