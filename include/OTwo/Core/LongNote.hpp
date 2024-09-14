#ifndef O2JAM_CORE_LONG_NOTE_HPP
#define O2JAM_CORE_LONG_NOTE_HPP

#include <OTwo/Core/Note.hpp>

class LongNote : public Note
{
public:
    explicit LongNote(const Chart::NoteEvent& ev);
    explicit LongNote(double position, double length, Chart::Channel channel = Chart::Channel::Background);

    bool IsVisible() const override;
    void SetVisible(const bool visible) override;

    double GetLength() const;
    void SetLength(double length);

    VerticesPtr GetHeadVertices() const;
    void SetHeadVertices(const std::array<sf::Vertex*, 6> &vertices);

    VerticesPtr GetTailVertices() const;
    void SetTailVertices(const std::array<sf::Vertex*, 6> &vertices);

    const Gx::Sprite* GetEdgePrefab(NoteShape) const;
    void SetEdgePrefabs(const PrefabMap& prefabs);

    void UpdateGeometry(const ChartRenderer& renderer, double delta) override;

private:
    VerticesPtr m_headVertices;
    VerticesPtr m_tailVertices;
    PrefabMap   m_edgePrefabs;
    double      m_length;

};

#endif
