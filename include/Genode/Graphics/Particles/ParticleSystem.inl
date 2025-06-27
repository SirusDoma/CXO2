#pragma once

namespace Gx
{
    template<typename T, typename ... Args>
    std::enable_if_t<std::is_base_of_v<ParticleEmitter, T>, ParticleEmitter&>
    ParticleSystem::AddEmitter(Args&&... args)
    {
        m_emitters.push_back(std::make_unique<T>(*this, std::forward<Args>(args)...));
        return *m_emitters.back();
    }
}
