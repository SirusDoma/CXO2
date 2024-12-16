#pragma once

#include <Genode/Graphics/SpriteBatch.hpp>

namespace Gx
{
    class RenderBatchContainer : public virtual RenderableContainer, public virtual UpdatableContainer
    {
    public:
        RenderBatchContainer() = default;

        void SetBatchMode(SpriteBatch::BatchMode batchMode) const;

    protected:
        void Update(double delta) override;
        RenderStates Render(RenderSurface& surface, RenderStates states) const override;

    private:
        // Batcher is kept separate so it doesn't interfere with SceneGraph hierarchy
        mutable SpriteBatch m_batcher{SpriteBatch::BatchMode::LayerSort};
    };
}
