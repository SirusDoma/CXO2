#pragma once

namespace Gx
{
    template<typename T>
    struct type_identity { using type = T; };

    template<typename T>
    using type_identity_t = typename type_identity<T>::type;
}