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
            T Next(T min = 0, T max = std::numeric_limits<T>::max())
            {
                auto randomizer = std::uniform_real_distribution<T>(min, max);
                return static_cast<T>(randomizer(m_seeder));
            }

            template<class T = int>
            static T Randomize(T min = 0, T max = std::numeric_limits<T>::max())
            {
                static auto device = std::random_device();
                static auto seeder = std::mt19937(device());

                if constexpr (std::is_floating_point_v<T>)
                {
                    auto randomizer    = std::uniform_real_distribution<T>(min, max);
                    return static_cast<T>(randomizer(seeder));
                }
                else
                {
                    auto randomizer    = std::uniform_int_distribution<T>(min, max);
                    return static_cast<T>(randomizer(seeder));
                }
            }

    private:
        std::random_device m_device;
        std::mt19937       m_seeder;
    };
}
