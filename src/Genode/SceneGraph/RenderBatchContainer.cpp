#include <Genode/SceneGraph/RenderBatchContainer.hpp>

namespace Gx
{
    void RenderBatchContainer::SetBatchMode(const SpriteBatch::BatchMode batchMode) const
    {
        m_batcher.SetBatchMode(batchMode);
    }

    void RenderBatchContainer::Update(const double delta)
    {
        m_batcher.Update(delta);
        UpdatableContainer::Update(delta);
    }

    RenderStates RenderBatchContainer::Render(RenderSurface& surface, RenderStates states) const
    {
        if (!IsVisible())
            return states;

        // Push render states
        const auto transform = states.transform;
        const float layer    = states.Layer;

        // Reset transform
        states.transform = sf::Transform();

        // Render child with sprite batch
        for (const auto node : GetChildren())
        {
            states.Layer += 1.f;
            if (const auto renderable = dynamic_cast<Renderable*>(node))
                renderable->Render(m_batcher, states);

            // Pop batch level
            states.Layer = layer;
        }

        // Pop transform
        states.transform = transform;

        // Render sprite batch
        return m_batcher.Render(surface, states);
    }
}