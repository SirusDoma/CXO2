#ifndef GENODE_ARCHIVE_HPP
#define GENODE_ARCHIVE_HPP

#include <Genode/System/Primitives.hpp>
#include <string>

namespace Gx
{
    class Archive
    {
    public:
        friend class ResourceContainer;

        virtual bool Contains(const std::string& name) = 0;
        virtual Int64 GetFile(const std::string& name, Uint8** data) const = 0;

        bool Open(const std::string& fileName);
        const std::string& GetFileName() const;
        const std::string& GetName() const;

    protected:
        Archive();
        virtual ~Archive();
        
    private:
        std::string m_filename;
    };
}

#endif