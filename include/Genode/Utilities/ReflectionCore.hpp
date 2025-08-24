#pragma once

#if defined(__APPLE__)
#include <Genode/Utilities/Extensions.hpp>

#include <type_traits>
#include <tuple>

namespace Gx
{
    namespace priv
    {
        constexpr unsigned int MaxParameterCount = 100;

        //-------------------------------------------------------------------------------
        template<class, std::size_t>
        struct any
        {
            template<class U>
            operator U() const;
        };

        //-------------------------------------------------------------------------------
        template<class T, size_t N>
        struct is_valid_amount
        {
            static std::false_type test(...);

            template<std::size_t... Is>
            static auto test(std::index_sequence<Is...>)
                -> decltype(T(any<T, Is>{}...), std::true_type{});

            static constexpr bool value =
                decltype(test(std::make_index_sequence<N>{}))::value;
        };

        template<class T, size_t N>
        inline constexpr bool is_valid_amount_v = is_valid_amount<T, N>::value;

        //-------------------------------------------------------------------------------
        template<class T, std::size_t N>
        struct ctor_def : std::integral_constant<std::size_t, N> {};

        template<class T, std::size_t N = MaxParameterCount>
        struct ctor_arg_count
                : std::conditional_t<is_valid_amount_v<T, N>,
                    ctor_def<T, N>,
                    std::conditional_t<N < 0, std::false_type, ctor_arg_count<T, N - 1>>> {};

        template<class T>
        inline constexpr std::size_t ctor_arg_count_v = ctor_arg_count<T>::value;
        //-------------------------------------------------------------------------------

        template<std::size_t N>
        struct tag {};

        template<typename T, std::size_t N>
        struct loophole_t
        {
            friend auto loophole(tag<N>) { return type_identity<T>{}; };
        };

        auto loophole(tag<0>);
        auto loophole(tag<1>);
        auto loophole(tag<2>);
        auto loophole(tag<3>);
        auto loophole(tag<4>);
        auto loophole(tag<5>);
        auto loophole(tag<6>);
        auto loophole(tag<7>);
        auto loophole(tag<8>);
        auto loophole(tag<9>);
        auto loophole(tag<10>);
        auto loophole(tag<11>);
        auto loophole(tag<12>);
        auto loophole(tag<13>);
        auto loophole(tag<14>);
        auto loophole(tag<15>);
        auto loophole(tag<16>);
        auto loophole(tag<17>);
        auto loophole(tag<18>);
        auto loophole(tag<19>);
        auto loophole(tag<20>);
        auto loophole(tag<21>);
        auto loophole(tag<22>);
        auto loophole(tag<23>);
        auto loophole(tag<24>);
        auto loophole(tag<25>);
        auto loophole(tag<26>);
        auto loophole(tag<27>);
        auto loophole(tag<28>);
        auto loophole(tag<29>);
        auto loophole(tag<30>);
        auto loophole(tag<31>);
        auto loophole(tag<32>);
        auto loophole(tag<33>);
        auto loophole(tag<34>);
        auto loophole(tag<35>);
        auto loophole(tag<36>);
        auto loophole(tag<37>);
        auto loophole(tag<38>);
        auto loophole(tag<39>);
        auto loophole(tag<40>);
        auto loophole(tag<41>);
        auto loophole(tag<42>);
        auto loophole(tag<43>);
        auto loophole(tag<44>);
        auto loophole(tag<45>);
        auto loophole(tag<46>);
        auto loophole(tag<47>);
        auto loophole(tag<48>);
        auto loophole(tag<49>);
        auto loophole(tag<50>);
        auto loophole(tag<51>);
        auto loophole(tag<52>);
        auto loophole(tag<53>);
        auto loophole(tag<54>);
        auto loophole(tag<55>);
        auto loophole(tag<56>);
        auto loophole(tag<57>);
        auto loophole(tag<58>);
        auto loophole(tag<59>);
        auto loophole(tag<60>);
        auto loophole(tag<61>);
        auto loophole(tag<62>);
        auto loophole(tag<63>);
        auto loophole(tag<64>);
        auto loophole(tag<65>);
        auto loophole(tag<66>);
        auto loophole(tag<67>);
        auto loophole(tag<68>);
        auto loophole(tag<69>);
        auto loophole(tag<70>);
        auto loophole(tag<71>);
        auto loophole(tag<72>);
        auto loophole(tag<73>);
        auto loophole(tag<74>);
        auto loophole(tag<75>);
        auto loophole(tag<76>);
        auto loophole(tag<77>);
        auto loophole(tag<78>);
        auto loophole(tag<79>);
        auto loophole(tag<80>);
        auto loophole(tag<81>);
        auto loophole(tag<82>);
        auto loophole(tag<83>);
        auto loophole(tag<84>);
        auto loophole(tag<85>);
        auto loophole(tag<86>);
        auto loophole(tag<87>);
        auto loophole(tag<88>);
        auto loophole(tag<89>);
        auto loophole(tag<90>);
        auto loophole(tag<91>);
        auto loophole(tag<92>);
        auto loophole(tag<93>);
        auto loophole(tag<94>);
        auto loophole(tag<95>);
        auto loophole(tag<96>);
        auto loophole(tag<97>);
        auto loophole(tag<98>);
        auto loophole(tag<99>);

        template<std::size_t N>
        struct detector
        {
            template<class T, std::size_t = sizeof(loophole_t<T, N>)>
            operator T();
        };

        template<typename T, typename Seq>
        struct ins;

        template<typename T, std::size_t... Is>
        struct ins<T, std::index_sequence<Is...> >
        {
            template<std::size_t = sizeof(T{detector<Is>{}...})>
            constexpr int operator()() const { return 0; }
        };

        template<std::size_t, std::size_t... Is, typename T = std::tuple<typename decltype(loophole(tag<Is>{}))::type...>>
        T get_type_impl(std::index_sequence<Is...>);

        template<typename T, std::size_t N = ctor_arg_count_v<T> >
        auto get_type()
        {
            return get_type_impl<ins<T, std::make_index_sequence<N> >{}()>(std::make_index_sequence<N>());
        }
    }

    template<typename T>
    constexpr int GetConstructorParameterCount() { return priv::ctor_arg_count_v<T>; }

    template<typename T>
    using GetConstructorDescriptor = decltype(priv::get_type<T>());

    template <typename T>
    struct Constructible
    {
        static constexpr bool value = std::is_default_constructible_v<T> || (
            !std::is_abstract_v<T> &&
            (
                GetConstructorParameterCount<T>() < priv::MaxParameterCount &&
                (
                    GetConstructorParameterCount<T>() != 1 ||
                    !std::is_same_v<GetConstructorDescriptor<T>, std::tuple<T>>
                )
            )
        );
    };
}
#endif
