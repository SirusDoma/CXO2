#include <Genode/System/Module.hpp>

namespace Gx
{
    Module::Module() :
        m_frequency(0),
        m_elapsed(0)
    {
    }

    Module::Module(double freq) :
        m_frequency(freq),
        m_elapsed(0)
    {
    }

    Module::~Module()
    {
    }

    double Module::GetFrequency() const
    {
        return m_frequency;
    }

    void Module::SetFrequency(double freq)
    {
        m_frequency = freq;
    }

    bool Module::CheckFrequency(double delta)
    {
        if (!m_frequency)
            return true;

        m_elapsed += delta;
        if (m_elapsed >= m_frequency)
            m_elapsed = 0;

        return m_elapsed == 0;
    }
}
