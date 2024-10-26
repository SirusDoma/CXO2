#pragma once

#include <tuple>
#include <utility>

// Based on
// * https://stackoverflow.com/a/54493136
//   https://godbolt.org/z/FxPDgU
// * http://alexpolt.github.io/type-loophole.html
//   https://github.com/alexpolt/luple/blob/master/type-loophole.h
//   by Alexandr Poltavsky, http://alexpolt.github.io
// * https://www.youtube.com/watch?v=UlNUNxLtBI0
//   Better C++14 reflections - Antony Polukhin - Meeting C++ 2018
// * Also special thanks to lapinozz

namespace Gx
{
    namespace priv
    {
        // The number of parameters supported.
        // See Constructible for more information.
        constexpr unsigned int MaxParameterCount = 100;

        // tag<T, N> generates friend declarations and helps with overload resolution.
        // There are two types: one with the auto return type, which is the way we read types later.
        // The second one is used in the detection of instantiations without which we'd get multiple
        // definitions.
        template <typename T, int N>
        struct tag {
            friend auto loophole(tag<T, N>);
            constexpr friend int cloophole(tag<T, N>);
        };

        // Helper class to temporarily initializes instance of U.
        template <typename U, typename = void>
        struct activator;

        template <typename U>
        struct activator<U, std::enable_if_t<std::is_default_constructible_v<U>>>;

        template <typename U>
        struct activator<U, std::enable_if_t<!std::is_default_constructible_v<U>>>;

        // The definitions of friend functions.
        template <typename T, typename U, int N, bool B,
                  typename = typename std::enable_if_t<
                    !std::is_same_v<
                      std::remove_cv_t<std::remove_reference_t<T>>,
                      std::remove_cv_t<std::remove_reference_t<U>>>>>
        struct fn_def {
            friend auto loophole(tag<T, N>) { return activator<U>::activate(); }
            constexpr friend int cloophole(tag<T, N>) { return 0; }
        };

        // This specialization is to avoid multiple definition errors.
        template <typename T, typename U, int N> struct fn_def<T, U, N, true> {};

        // This has a templated conversion operator which in turn triggers instantiations.
        // Important point, using sizeof seems to be more reliable. Also default template
        // arguments are "cached" (I think). To fix that I provide a U template parameter to
        // the ins functions which do the detection using constexpr friend functions and SFINAE.
        template <typename T, int N>
        struct c_op {
            template <typename U, int M>
            static auto ins(...) -> int;
            template <typename U, int M, int = cloophole(tag<T, M>{})>
            static auto ins(int) -> char;

            template <typename U, int = sizeof(fn_def<T, U, N, sizeof(ins<U, N>(0)) == sizeof(char)>)>
            operator U*();

            template <typename U, int = sizeof(fn_def<T, U, N, sizeof(ins<U, N>(0)) == sizeof(char)>)>
            operator U&();
        };

        // This is a helper to turn a ctor into a tuple type.
        // Usage is: refl::as_tuple<data_t>
        template <typename T, typename U> struct loophole_tuple;

        // This is a helper to turn a ctor into a tuple type.
        template <typename T, int... Ns>
        struct loophole_tuple<T, std::integer_sequence<int, Ns...>> {
            using type = std::tuple<decltype(loophole(tag<T, Ns>{}))...>;
        };
    }

    // Here is a version of fields_number to handle user-provided ctor.
    // NOTE: It finds the first ctor having the shortest unambigious set
    //       of parameters.
    template <typename T, int... Ns>
    constexpr auto GetConstructorParameterCount(int) -> decltype(T(priv::c_op<T, Ns>{}...), 0)
    {
        return sizeof...(Ns);
    }

    template <typename T, int... Ns>
    constexpr int GetConstructorParameterCount(...)
    {
        if constexpr (sizeof...(Ns) < priv::MaxParameterCount)
            return GetConstructorParameterCount<T, Ns..., sizeof...(Ns)>(0);
        else
            return sizeof...(Ns);
    }

    template <typename T, int... Ns>
    constexpr int GetConstructorParameterCount(std::integral_constant<int, priv::MaxParameterCount>)
    {
        return sizeof...(Ns);
    }

    // Usage is: Gx::ConstructorDescriptor<MyClass>
    template <typename T>
    using ConstructorDescriptor = typename priv::loophole_tuple<T, std::make_integer_sequence<int, GetConstructorParameterCount<T>(0)>>::type;

    template <typename T>
    struct Constructible
    {
        // GetConstructorParameterCount will repeatedly recurse itself infinitely when a given type has no public constructor.
        // Use this as an advantage to determine whether the class can be publicly constructible by setting a depth limit to the recursive.
        static constexpr bool value = !std::is_abstract_v<T> && (std::is_default_constructible_v<T> || GetConstructorParameterCount<T>(0) < priv::MaxParameterCount);
    };

    namespace priv
    {
        template <typename U>
        struct activator<U, std::enable_if_t<std::is_default_constructible_v<U>>>
        {
            static U activate() { return U(); }
        };

        template <typename U>
        struct activator<U, std::enable_if_t<!std::is_default_constructible_v<U>>>
        {
            static U activate()
            {
                auto parameters = ConstructorDescriptor<U>(); // temp reference, will be invalid but not used anyway
                return std::apply([](auto&... args) -> U { return U(std::forward<decltype(args)>(args)...); }, parameters);
            }
        };
    }
}
