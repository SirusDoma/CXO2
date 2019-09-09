#ifndef GENODE_RESOURCE_DEFINITION_HPP
#define GENODE_RESOURCE_DEFINITION_HPP

#include <string>
#include <vector>

namespace Gx
{
    class ResourceDefinition
    {
    public:
        virtual ~ResourceDefinition() {}
        
        std::string Type;

        // TODO: Make it into ResourceDependencies
        std::string Texture;
        std::string Font;
    };
}

#endif