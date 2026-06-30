#pragma once

#include <CXO2/Core/Chart.hpp>
#include <CXO2/Config/GameConfig.hpp>

#include <CXO2/Core/NoteFactory.hpp>
#include <CXO2/Core/NoteGuideLine.hpp>

#include <Genode/Entities/Renderable.hpp>
#include <Genode/Graphics/Sprite.hpp>

namespace Cx
{
    static constexpr unsigned int DefaultMeasureHeight = 384;

    class ChartRenderer;
    class Note : public Gx::Renderable
    {
    public:
        explicit Note(const Chart::NoteEvent& ev);
        explicit Note(double position, Chart::Channel channel = Chart::Channel::Background);

        double GetRenderPosition() const;
        void SetRenderPosition(double position);

        Chart::Channel GetChannel() const;
        void SetChannel(Chart::Channel channel);

        NoteGuideLine* GetGuideLine();

        const Gx::VertexSpan& GetVertices() const;
        void SetVertices(Gx::VertexSpan&& vertices) noexcept;

        const Gx::Sprite* GetPrefab(NoteShape shape) const;
        void SetPrefabs(const PrefabMap& prefabs);

        bool IsVisible() const override;
        void SetVisible(const bool visible) override;

        virtual void UpdateGeometry(const ChartRenderer& renderer, double delta);

    protected:
        Gx::VertexSpan& GetVertices();

        static void UpdatePositions(Gx::VertexSpan& span, const sf::Vector2f& position, const sf::FloatRect& bounds);
        static void UpdateTexCoords(Gx::VertexSpan& span, const sf::IntRect& texcoords);

    private:
        Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

        std::optional<Gx::VertexSpan> m_span;
        PrefabMap      m_prefabs;
        double         m_position;
        Chart::Channel m_channel;
        NoteGuideLine  m_line;
    };
}
