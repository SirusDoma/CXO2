#ifndef O2JAM_PROGRESS_BAR_METADATA_HPP
#define O2JAM_PROGRESS_BAR_METADATA_HPP

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/Json.hpp>
#include <Genode/UI/ProgressBar.hpp>

#include <O2/Metadata/SpriteMetadata.hpp>

struct ProgressBarMetadata : public SpriteMetadata
{
public:
    float GetMaximum() const { return m_maximum; }
    void SetMaximum(float maximum) { m_maximum = maximum; }

    Gx::ProgressBar::Orientation GetOrientation() const { return m_orientation; }
    void SetOrientation(Gx::ProgressBar::Orientation orientation) { m_orientation = orientation; }

private:
    float m_maximum;
    Gx::ProgressBar::Orientation m_orientation;
};

#endif