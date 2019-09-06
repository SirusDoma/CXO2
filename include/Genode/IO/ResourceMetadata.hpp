#ifndef GENODE_RESOURCE_METADATA_HPP
#define GENODE_RESOURCE_METADATA_HPP

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System.hpp>

#include <string>
#include <vector>

namespace Gx
{
	struct ResourceMetadata
	{
		struct FrameInfo
		{
			sf::IntRect  TexCoords;
			sf::Vector2f Position;
			sf::Vector2f Scale;
			float        Rotation;
		};

		std::string Texture;
		std::string Font;
		std::vector<FrameInfo> Frames;
		std::string Text;
	};
}

#endif