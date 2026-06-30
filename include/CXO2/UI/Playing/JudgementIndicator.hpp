#pragma once

#include <CXO2/Models/Game.hpp>

#include <Genode/SceneGraph/Node.hpp>
#include <Genode/Entities/Renderable.hpp>
#include <Genode/Entities/Updatable.hpp>
#include <Genode/Graphics/Animation.hpp>
#include <Genode/Tween/Scale.hpp>

#include <unordered_map>

namespace Gx { class ResourceManager; }

namespace Cx
{
    class JudgementIndicator : public Gx::Node, public Gx::Renderable, public Gx::Updatable
    {
    public:
        explicit JudgementIndicator(const std::unordered_map<Accuracy, Gx::Animation*>& indicators, bool useFx = true);

        void Initialize() override;
        void Play(Accuracy accuracy);

    private:
        void Update(const double delta) override;
        Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

        std::unordered_map<Accuracy, Gx::Animation*> m_indicators;
        bool m_useFx;

        double m_elapsed;
        Gx::Animation* m_target;
        std::optional<Gx::Scale> m_scale;
    };
}
