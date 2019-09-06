#ifndef STATE_LOGO_HPP
#define STATE_LOGO_HPP

#include <Genode/SceneGraph.hpp>
#include <Genode/Graphics.hpp>

class StateLogo : public Gx::Scene
{
private:
	sf::Texture texture;
	Gx::Sprite* sprite;
public:
	virtual void Stage()
	{
		texture.loadFromFile("D:\\O2Jam\\Assets\\NX\\State_Planet\\State_Planet.png");
		texture.setSmooth(true);

		sprite = new Gx::Sprite(texture);

		AddChild(sprite);
	}
};

#endif