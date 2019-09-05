#ifndef GENODE_RESOURCE_HEADER
#define GENODE_RESOURCE_HEADER

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System.hpp>
#include <string>

struct ResourceHeader
{
	struct FrameInfo
	{
		sf::IntRect  TexCoords;
		sf::Vector2f Position;
		sf::Vector2f Scale;
		float        Rotation;
	};

	std::string Texture;
	FrameInfo   Frames[];
};

#endif