#pragma once

#include <Genode/Entities/Renderable.hpp>

class Note;
class LongNote;
class ChartRenderer;
class NoteGuideLine : public Gx::Renderable
{
public:
    NoteGuideLine(const NoteGuideLine& copy);
    explicit NoteGuideLine(const Note& parent);
    explicit NoteGuideLine(const LongNote& parent);

    bool IsVisible() const override;
    void SetVisible(const bool visible) override;

    const std::array<sf::Vertex*, 8>& GetVertices() const;
    void SetVertices(const std::array<sf::Vertex*, 8>& vertices);

    virtual void Render(const ChartRenderer& renderer, double delta);

private:
    Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

    const Note* m_parent;
    double m_guideLength;
    double m_delta;
    std::array<sf::Vertex*, 8> m_vertices;
};
