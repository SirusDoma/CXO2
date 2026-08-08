#pragma once

#include <CXO2/Events/Interceptable.hpp>
#include <CXO2/Config/GameConfig.hpp>

#include <Genode/Events/EventDispatcher.hpp>

namespace Cx
{
    class OptionDialog;

    enum class OptionEventKey
    {
        OnInitialize,
        OnSave,
        OnReset
    };

    struct OptionEventArgs : Interceptable {};

    struct OptionConfigEventArgs : Interceptable
    {
        GameConfig& Config;

        explicit OptionConfigEventArgs(GameConfig& config) : Config(config) {}
    };

    using OptionEvent       = Gx::Event<OptionEventKey, OptionDialog, OptionEventArgs>;
    using OptionConfigEvent = Gx::Event<OptionEventKey, OptionDialog, OptionConfigEventArgs>;

    namespace OptionEvents
    {
        inline constexpr OptionEvent       OnInitialize{OptionEventKey::OnInitialize};
        inline constexpr OptionConfigEvent OnSave{OptionEventKey::OnSave};
        inline constexpr OptionConfigEvent OnReset{OptionEventKey::OnReset};
    }
}
