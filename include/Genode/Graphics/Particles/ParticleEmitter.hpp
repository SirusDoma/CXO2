#pragma once

#include <Genode/Entities/Updatable.hpp>
#include <Genode/Graphics/VertexPool.hpp>
#include <Genode/Graphics/Particles/Particle.hpp>

#include <SFML/System/Time.hpp>

#include <optional>
#include <functional>
#include <memory>

namespace Gx
{
    class ParticleSystem;
    class ParticleEmitter : Updatable
    {
    public:
        using EmissionTrigger = std::function<std::unique_ptr<Particle>(Poolable<VertexSpan>& vertices)>;
        using ParticleList = std::vector<std::unique_ptr<Particle>>;

        explicit ParticleEmitter(ParticleSystem& particleSystem);
        ParticleEmitter(
            ParticleSystem& particleSystem,
            std::size_t count,
            sf::Time interval,
            std::optional<std::size_t> repeatCount,
            const EmissionTrigger& trigger
        );

        [[nodiscard]] ParticleSystem& GetParticleSystem() const;
        [[nodiscard]] std::size_t GetEmissionCount() const;
        [[nodiscard]] sf::Time GetEmissionInterval() const;
        [[nodiscard]] std::optional<std::size_t> GetEmissionRepeatCount() const;

        void SetEmissionRate(std::size_t count, sf::Time interval, std::optional<std::size_t> repeatCount);
        void SetEmissionTrigger(const EmissionTrigger& trigger);

        virtual void Emit();
        void Clear();

        void Update(double delta) override;

        [[nodiscard]] std::vector<Particle*> GetParticles() const;

    private:
        ParticleSystem&            m_system;
        std::size_t                m_count       = {};
        std::size_t                m_emitCount   = 0;
        sf::Time                   m_interval    = sf::Time::Zero;
        double                     m_elapsed     = {};
        std::optional<std::size_t> m_repeatCount = 0;
        EmissionTrigger            m_trigger     = {};
        ParticleList               m_particles   = {};
    };
}
