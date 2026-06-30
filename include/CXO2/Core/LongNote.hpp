#pragma once

#include <CXO2/Core/Note.hpp>

namespace Cx
{
    class LongNote : public Note
    {
    public:
        explicit LongNote(const Chart::NoteEvent& ev);
        explicit LongNote(double position, double length, Chart::Channel channel = Chart::Channel::Background);

        bool IsVisible() const override;
        void SetVisible(const bool visible) override;

        double GetLength() const;
        void SetLength(double length);

        Gx::VertexSpan& GetHeadVertices();
        void SetHeadVertices(Gx::VertexSpan&& vertices);

        Gx::VertexSpan& GetTailVertices();
        void SetTailVertices(Gx::VertexSpan&& vertices);

        const Gx::Sprite* GetEdgePrefab(NoteShape) const;
        void SetEdgePrefabs(const PrefabMap& prefabs);

        void UpdateGeometry(const ChartRenderer& renderer, double delta) override;

    private:
        std::optional<Gx::VertexSpan> m_headVertices;
        std::optional<Gx::VertexSpan> m_tailVertices;
        PrefabMap                     m_edgePrefabs;
        double                        m_length;

    };
}
