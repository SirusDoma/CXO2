
#include "O2JamResourceLoader.hpp"

template<typename R>
bool O2JamResourceLoader<R>::IsMetadataRequired() const
{
    return true;
}

template<typename R>
bool O2JamResourceLoader<R>::IsStreamed() const
{
    return false;
}

template<typename R>
Gx::ResourcePtr<R> O2JamResourceLoader<R>::Load(const void* data, std::size_t size) const
{
    return Load(*LoadMetadata(data, size));
}

template<typename R>
void O2JamResourceLoader<R>::ParseReferences(Json require, Gx::ResourceMetadata &metadata)
{
    if (require.empty())
        return;

    for (auto resource : require.items())
    {
        if (resource.key() == "texture")
            metadata.RequireTexture(resource.key(), resource.value());
        else if (resource.key() == "font")
            metadata.RequireFont(resource.key(), resource.value());
        else if (resource.key() == "sound")
            metadata.RequireSoundBuffer(resource.key(), resource.value());
        else
            metadata.Require(resource.key(), resource.value(), Gx::ResourceReference::Custom);
    }
}

template<typename R>
O2JamResourceLoader<R>::O2JamResourceLoader() :
    Gx::ResourceLoader<R>()
{
}
