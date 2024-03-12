#ifndef O2JAM_CHAT_WINDOW_METADATA_HPP
#define O2JAM_CHAT_WINDOW_METADATA_HPP

#include <OTwo/Metadata/Graphics/TransformMetadata.hpp>

#include <SFML/Graphics/Rect.hpp>

struct ChatWindowMetadata : public TransformMetadata
{
    sf::FloatRect Bounds;
    unsigned int  FontSize;
    unsigned int  MaximumChatsLength;
};

#endif
