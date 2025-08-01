#pragma once

#include <Genode/IO/ResourceManager.hpp>
#include <string>

class TextureCacheBuilder
{
public:
    TextureCacheBuilder(Gx::Archive& archive, Gx::ResourceManager& resources);

    void BuildCache() const;

private:
    void BuildInterfaceCache() const;
    void BuildPlayingCache() const;
    void BuildItemCache() const;

    void BuildTextures() const;
    void BuildControlsCache(const std::string& controlListName, bool mapBounds = true) const;

    Gx::Archive& m_archive;
    Gx::ResourceManager& m_resources;
};
