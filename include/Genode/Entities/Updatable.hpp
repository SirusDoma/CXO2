#pragma once

namespace Gx
{
    class Updatable
    {
    public:
        virtual void Update(double delta) = 0;
        virtual ~Updatable() = default;
    };
}
