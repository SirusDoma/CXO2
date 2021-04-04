#ifndef REPEATER_METADATA_HPP
#define REPEATER_METADATA_HPP

#include <O2/Metadata/TransformMetadata.hpp>

struct RepeaterMetadata : TransformMetadata
{
public:
    int GetVerticalCount() const { return m_verticalCount; }
    void SetVerticalCount(int verticalCount) { m_verticalCount = verticalCount; }

    int GetHorizontalCount() const { return m_horizontalCount; }
    void SetHorizontalCount(int horizontalCount) { m_horizontalCount = horizontalCount; }

    float GetVerticalSpacing() const { return m_verticalSpacing; }
    void SetVerticalSpacing(float verticalSpacing) { m_verticalSpacing = verticalSpacing; }

    float GetHorizontalSpacing() const { return m_horizontalSpacing; }
    void SetHorizontalSpacing(float horizontalSpacing) { m_horizontalSpacing = horizontalSpacing; }

private:
    int m_verticalCount, m_horizontalCount;
    float m_verticalSpacing, m_horizontalSpacing;
};

#endif
