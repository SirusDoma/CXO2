#ifndef PROGRESS_BAR_METADATA_HPP
#define PROGRESS_BAR_METADATA_HPP

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/Json.hpp>
#include <Genode/UI/ProgressBar.hpp>

#include <O2/IO/Metadata/SpriteMetadata.hpp>

struct ProgressBarMetadata : public SpriteMetadata
{
    float Maxiumum;
    Gx::ProgressBar::Orientation Orientation;
};

#endif