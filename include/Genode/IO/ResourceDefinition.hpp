#ifndef GENODE_RESOURCE_DEFINITION_HPP
#define GENODE_RESOURCE_DEFINITION_HPP

#include <string>
#include <vector>
#include <unordered_map>

namespace Gx
{
    class ResourceDefinition
    {
    public:
        virtual ~ResourceDefinition() {}
        
        std::string Type;

        std::unordered_map<std::string, std::string> ResourceReferences;
    };
}

#endif