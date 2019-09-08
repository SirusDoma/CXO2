#ifndef GENODE_RESOURCE_DEFINITION_HPP
#define GENODE_RESOURCE_DEFINITION_HPP

#include <string>
#include <vector>

namespace Gx
{
    class ResourceDefinition
    {
    public:
        ResourceDefinition();
        virtual ~ResourceDefinition();
        
        std::string Texture;
        std::string Font;
    };
}

#endif