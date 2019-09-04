#include <Genode/SceneGraph.hpp>
#include <Genode/Graphics.hpp>

namespace Nx
{
    class SplashScreen : public Gx::Scene
    {
    private:
        sf::Texture texture;
        Gx::Sprite* sprite;
    public:
        virtual void Stage()
        {
            texture.loadFromFile("D:\\final-fantasy-xv.jpg");
            sprite = new Gx::Sprite(texture);

            AddChild(sprite);
        }
    };
}