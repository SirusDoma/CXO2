#pragma once

#include <random>

namespace Gx
{
    class Randomizer
    {
        public:
            Randomizer() :
                m_device(std::random_device()),
                m_seeder(m_device())
            {
            }

            template<class T = int>
            T Next(T min = 0, T max = std::numeric_limits<T>::max)
            {
                auto randomizer = std::uniform_int_distribution<T>(min, max);
                return randomizer(m_seeder);
            }

            template<class T = int>
            static T Randomize(T min = 0, T max = std::numeric_limits<T>::max)
            {
                auto device      = std::random_device();
                auto seeder      = std::mt19937(device());
                auto randomizer  = std::uniform_int_distribution<T>(min, max);

                return randomizer(seeder);
            }

    private:
        std::random_device m_device;
        std::mt19937       m_seeder;
    };
}
