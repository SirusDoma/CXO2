#include <Genode/Graphics/Particles/ParticleSystem.hpp>

#include <algorithm>

namespace Gx
{
    ParticleSystem::ParticleSystem(const sf::PrimitiveType primitiveType) :
        m_primitive(primitiveType)
    {
    }

    ParticleEmitter& ParticleSystem::AddEmitter(const ParticleEmitter::EmissionTrigger& emitter)
    {
        auto particleEmitter = std::make_unique<ParticleEmitter>(*this);
        particleEmitter->SetEmissionTrigger(emitter);
        m_emitters.push_back(std::move(particleEmitter));

        return *m_emitters.back();
    }

    ParticleEmitter& ParticleSystem::AddEmitter(
        std::size_t count,
        sf::Time interval,
        std::optional<std::size_t> repeatCount,
        const ParticleEmitter::EmissionTrigger& emitter
    )
    {
        auto particleEmitter = std::make_unique<ParticleEmitter>(*this, count, interval, repeatCount, emitter);
        m_emitters.push_back(std::move(particleEmitter));

        return *m_emitters.back();
    }

    const ParticleSystem::EmitterList& ParticleSystem::GetEmitters() const
    {
        return m_emitters;
    }

    void ParticleSystem::RemoveEmitter(ParticleEmitter& emitter)
    {
        m_emitters.erase(std::remove_if(m_emitters.begin(), m_emitters.end(), [&] (auto& e)
        {
            return e.get() == &emitter;
        }), m_emitters.end());
    }

    void ParticleSystem::ClearEmitters()
    {
        m_emitters.clear();
    }

    const sf::Texture* ParticleSystem::GetTexture() const
    {
        return m_texture;
    }

    void ParticleSystem::SetTexture(const sf::Texture& texture)
    {
        m_texture = &texture;
    }

    void ParticleSystem::Emit() const
    {
        for (const auto& emitter : m_emitters)
            emitter->Emit();
    }

    VertexSpan ParticleSystem::Rent(const std::size_t count)
    {
        return m_vertices.Rent(count);
    }

    void ParticleSystem::Return(VertexSpan& span)
    {
        m_vertices.Return(span);
    }

    RenderStates ParticleSystem::Render(RenderSurface& surface, RenderStates states) const
    {
        states.texture = m_texture;
        states.transform *= GetTransform();

        surface.Render(m_vertices.GetData(), m_vertices.GetSize(), m_primitive, states);

        return RenderableContainer::Render(surface, states);
    }

    void ParticleSystem::Update(const double delta)
    {
        for (const auto& emitter : m_emitters)
            emitter->Update(delta);

        UpdatableContainer::Update(delta);
    }
}
