#pragma once
#include <Genode/IO/ResourceLoader.hpp>
#include <CXO2/IO/TextureCompiler.hpp>

namespace Cx
{
    class O2JamSpriteLoader : public Gx::ResourceLoader<SpriteSheet>
    {
    public:
        Gx::ResourcePtr<SpriteSheet> LoadFromFile(const std::string& fileName, const Gx::ResourceContext& ctx) const override;
        Gx::ResourcePtr<SpriteSheet> LoadFromMemory(void* data, std::size_t size, const Gx::ResourceContext& ctx) const override;
        Gx::ResourcePtr<SpriteSheet> LoadFromStream(sf::InputStream& stream, const Gx::ResourceContext& ctx) const override;
    };

}
