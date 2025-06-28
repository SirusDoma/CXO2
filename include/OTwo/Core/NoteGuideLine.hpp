#pragma once

#include <Genode/Entities/Renderable.hpp>
#include <Genode/Graphics/VertexPool.hpp>

class Note;
class LongNote;
class ChartRenderer;
class NoteGuideLine : public Gx::Renderable
{
public:
    // NoteGuideLine(const NoteGuideLine& copy);
    explicit NoteGuideLine(const Note& parent);
    explicit NoteGuideLine(const LongNote& parent);

    bool IsVisible() const override;
    void SetVisible(const bool visible) override;

    const Gx::VertexSpan& GetVertices() const;
    void SetVertices(Gx::VertexSpan&& vertices) noexcept;

    virtual void Render(const ChartRenderer& renderer, double delta);

private:
    Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

    const Note* m_parent;
    double m_guideLength;
    double m_delta;

    std::optional<Gx::VertexSpan> m_span;
};
