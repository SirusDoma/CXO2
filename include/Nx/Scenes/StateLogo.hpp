#ifndef STATE_LOGO_HPP
#define STATE_LOGO_HPP

#include <Genode/SceneGraph.hpp>
#include <Genode/Graphics.hpp>
#include <Genode/IO.hpp>

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
        Gx::Uint64 size = Gx::FileHelper::GetFile("D:\\O2Jam\\Assets\\NX\\State_Planet\\State_Planet.png", &bytes);

        sprite = new Gx::Sprite(cache->Add<sf::Texture>("State_Planet", bytes, size));

        AddChild(sprite);
    }
};

#endif