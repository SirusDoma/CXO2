#ifndef STATE_LOGO_HPP
#define STATE_LOGO_HPP

#include <Genode/SceneGraph.hpp>
#include <Genode/Graphics.hpp>
#include <Genode/IO.hpp>

#include <Nx/IO/Definitions/SpriteDefinition.hpp>
#include <Nx/IO/Definitions/SpriteDefinitionLoader.hpp>

class StateLogo : public Gx::Scene
{
private:
    sf::Texture texture;
    Gx::Sprite* sprite;

public:
    virtual void Initialize()
    {
        auto cache = Gx::CacheManager::Instance();
        Gx::Uint8* bytes;
        Gx::Uint64 size;

        // [Test] Manually load Sprite Definition
        size = Gx::FileHelper::GetFile("D:\\Projects\\Repos\\Personal\\O2Jam\\assets\\samples\\definitions\\sprite.new.json", &bytes);
        auto loader = SpriteDefinitionLoader();
        Gx::ResourceLoaderFactory::Register<Gx::Sprite>(new SpriteDefinitionLoader());
        Gx::CacheManager::Instance()->Add("State_Planet/Definition", loader.Load(bytes, size));

        // Create sprite from definition (!!!)
        sprite = Gx::ResourceManager::Instance()->Create<Gx::Sprite>("State_Planet/Definition");
        AddChild(sprite);

        // Even the caches are cleared, the existing sprite won't be affected
        Gx::CacheManager::Instance()->Remove("State_Planet/Definition");
        Gx::CacheManager::Instance()->Remove("D:\\O2Jam\\Assets\\NX\\State_Planet\\State_Planet.png");
    }
};

#endif