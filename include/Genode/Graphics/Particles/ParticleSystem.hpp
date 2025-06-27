#pragma once

#include <Genode/Graphics/VertexPool.hpp>
#include <Genode/Graphics/Particles/ParticleEmitter.hpp>

#include <Genode/SceneGraph/Node.hpp>
#include <Genode/SceneGraph/RenderableContainer.hpp>
#include <Genode/SceneGraph/UpdatableContainer.hpp>

#include <SFML/Graphics.hpp>

#include <functional>
#include <vector>

namespace Gx
{
    class ParticleSystem : public virtual Node, public RenderableContainer,
                           public UpdatableContainer, public Poolable<VertexSpan>
    {
    public:
        using EmitterList = std::vector<std::unique_ptr<ParticleEmitter>>;

        explicit ParticleSystem(sf::PrimitiveType primitiveType = sf::PrimitiveType::Triangles);

        template<typename T, typename ... Args>
        std::enable_if_t<std::is_base_of_v<ParticleEmitter, T>, ParticleEmitter&>
        AddEmitter(Args&&... args);

        ParticleEmitter& AddEmitter(const ParticleEmitter::EmissionTrigger& emitter);

        ParticleEmitter& AddEmitter(
            std::size_t count,
            sf::Time interval,
            std::optional<std::size_t> repeatCount,
            const ParticleEmitter::EmissionTrigger& emitter
        );

        const EmitterList& GetEmitters() const;

        void RemoveEmitter(ParticleEmitter& emitter);

        void ClearEmitters();

        [[nodiscard]] const sf::Texture* GetTexture() const;
        void SetTexture(const sf::Texture& texture);

        void Emit() const;

        RenderStates Render(RenderSurface& surface, RenderStates states) const override;
        void Update(double delta) override;

    private:
        [[nodiscard]] VertexSpan Rent(std::size_t count) override;
        void Return(VertexSpan& span) override;

        sf::PrimitiveType  m_primitive = sf::PrimitiveType::Triangles;
        VertexPool         m_vertices  = {};
        EmitterList        m_emitters  = {};
        const sf::Texture* m_texture   = nullptr;
    };


}

#include <Genode/Graphics/Particles/ParticleSystem.inl>
