#ifndef O2JAM_PROGRESS_BAR_METADATA_HPP
#define O2JAM_PROGRESS_BAR_METADATA_HPP

#include <OTwo/Metadata/ResourceMetadata.hpp>
#include <OTwo/Metadata/Graphics/SpriteMetadata.hpp>

#include <Genode/IO/Json.hpp>
#include <Genode/UI/ProgressBar.hpp>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

struct ProgressBarMetadata : public SpriteMetadata
{
    float Maximum;
    Gx::ProgressBar::Orientation Orientation;
};

#endif