#include <Genode/Graphics/Particles/ParticleEmitter.hpp>
#include <Genode/Graphics/Particles/ParticleSystem.hpp>

#include <algorithm>

namespace Gx
{
    ParticleEmitter::ParticleEmitter(ParticleSystem& particleSystem) :
        m_system(particleSystem)
    {
    }

    ParticleEmitter::ParticleEmitter(
        ParticleSystem& particleSystem,
        const std::size_t count,
        const sf::Time interval,
        const std::optional<std::size_t> repeatCount,
        const EmissionTrigger& trigger
    ) :
        m_system(particleSystem)
    {
        SetEmissionRate(count, interval, repeatCount);
        SetEmissionTrigger(trigger);

        m_elapsed = interval.asMilliseconds();
    }

    ParticleSystem& ParticleEmitter::GetParticleSystem() const
    {
        return m_system;
    }

    std::size_t ParticleEmitter::GetEmissionCount() const
    {
        return m_count;
    }

    sf::Time ParticleEmitter::GetEmissionInterval() const
    {
        return m_interval;
    }

    std::optional<std::size_t> ParticleEmitter::GetEmissionRepeatCount() const
    {
        return m_repeatCount;
    }

    void ParticleEmitter::SetEmissionRate(std::size_t count, sf::Time interval, std::optional<std::size_t> repeatCount)
    {
        m_count       = count;
        m_interval    = interval;
        m_repeatCount = repeatCount;

        m_elapsed   = 0;
        m_emitCount = 0;
    }

    void ParticleEmitter::SetEmissionTrigger(const EmissionTrigger& trigger)
    {
        m_trigger = trigger;
    }

    void ParticleEmitter::Emit()
    {
        if (m_trigger)
        {
            m_elapsed = 0;
            m_emitCount++;

            for (std::size_t i = 0; i < m_count; i++)
            {
                m_particles.push_back(m_trigger(m_system));
            }
        }
    }

    void ParticleEmitter::Clear()
    {
        for (const auto& particle : m_particles)
            particle->Destroy();

        m_particles.clear();
    }

    void ParticleEmitter::Update(const double delta)
    {
        if (m_trigger && (!m_repeatCount.has_value() || m_emitCount <= *m_repeatCount))
        {
            m_elapsed += delta;
            if (sf::milliseconds(static_cast<std::int32_t>(m_elapsed)) >= m_interval)
                Emit();
        }

        for (auto it = m_particles.begin(); it != m_particles.end();)
        {
            const auto& particle = *it;

            particle->Update(delta);
            if (!particle->IsActive())
                it = m_particles.erase(it);
            else
                ++it;
        }
    }

    std::vector<Particle*> ParticleEmitter::GetParticles() const
    {
        auto particles = std::vector<Particle*>();
        std::transform(m_particles.begin(), m_particles.end(), std::back_inserter(particles),
            [](const auto& ptr) { return ptr.get(); });

        return particles;
    }
}
