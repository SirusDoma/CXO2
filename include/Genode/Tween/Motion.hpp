#pragma once

#include <functional>

namespace Gx
{
    using MotionFunc = std::function<float(float)>;

    class Motion final
    {
    public:
        Motion() = delete;

        static MotionFunc CubicBerzier(const float x1, const float x2, const float y1, const float y2)
        {
            return [=] (const float t)
            {
                if (t <= 0.f) return 0.f;
                if (t >= 1.f) return 1.f;

                float u = t;
                constexpr int ITERATIONS = 10;
                for (int i = 0; i < ITERATIONS; ++i)
                {
                    const float oneMinus = 1.f - u;
                    const float f = 3.f * oneMinus * oneMinus * u * x1 +
                                    3.f * oneMinus * u * u * x2 +
                                    u * u * u - t;

                    if (std::fabs(f) < 1e-6f)
                        break;

                    const float df = 3.f * oneMinus * oneMinus * x1 +
                                     6.f * oneMinus * u * (x2 - x1) +
                                     3.f * u * u * (1.f - x2);

                    if (std::fabs(df) < 1e-6f)
                        break;

                    u -= f / df;
                }

                const float oneMinus = 1.f - u;
                const float y = 3.f * oneMinus * oneMinus * u * y1 +
                                3.f * oneMinus * u * u * y2 +
                                u * u * u;
                return y;
            };
        }


        inline static MotionFunc Linear = [] (const float t) { return t; };

        inline static MotionFunc Ease      = CubicBerzier(0.42f, 0.f, 1.f, 1.f);
        inline static MotionFunc EaseIn    = CubicBerzier(0.42f, 0.f, 1.f, 1.f);
        inline static MotionFunc EaseOut   = CubicBerzier(0.42f, 0.f, 0.58f, 1.f);
        inline static MotionFunc EaseInOut = CubicBerzier(0.f, 0.f, 0.58f, 1.f);
        inline static MotionFunc Bouncy    = CubicBerzier(0.175f, 0.885f, 0.32f, 1.275f);


    };
}