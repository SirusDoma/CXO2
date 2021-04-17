#ifndef GENODE_SYSTEM_MODULE_HPP
#define GENODE_SYSTEM_MODULE_HPP

#include <Genode/Entities/Updatable.hpp>
#include <SFML/Window/Event.hpp>

namespace Gx
{
    class Module : public virtual Updatable
    {
    public:
        virtual ~Module();

        double GetFrequency() const;
        void SetFrequency(double freq);

    protected:
        Module();
        Module(double freq);

    private:
        friend class Application;
        bool CheckFrequency(double delta);

        double m_frequency;
        double m_elapsed;
    };
}

#endif