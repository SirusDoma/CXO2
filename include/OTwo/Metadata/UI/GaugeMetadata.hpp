#ifndef O2JAM_GAUGE_METADATA_HPP
#define O2JAM_GAUGE_METADATA_HPP

#include <OTwo/Metadata/ResourceMetadata.hpp>
#include <OTwo/Metadata/Graphics/SpriteMetadata.hpp>

#include <Genode/IO/Json.hpp>
#include <Genode/UI/Gauge.hpp>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

struct GaugeMetadata : public SpriteMetadata
{
    float Maximum;
    Gx::Gauge::Orientation Orientation;
};

#endif