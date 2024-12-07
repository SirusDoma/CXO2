#pragma once

namespace Gx
{
    template<typename T>
    T& Scene::Require()
    {
        return GetContext().Require<T>();
    }
}