#include <Extensions/AccuracyMeterExtension.hpp>
#include <Log.hpp>
#include <Ui.hpp>

#include <CXO2/Constants/Identifiers/ChatPanel.hpp>
#include <CXO2/Constants/Identifiers/Playing7K.hpp>

#include <Genode/UI/Image.hpp>
#include <Genode/UI/InputField.hpp>
#include <Genode/UI/UiContainer.hpp>

#include <fmt/format.h>

#include <algorithm>
#include <cmath>

namespace O2CX
{
    using namespace Cx::Constants::Identifiers;

    namespace
    {
        constexpr double CoolWindow = 6.0 / 192.0;
        constexpr double GoodWindow = 18.0 / 192.0;
        constexpr double BadWindow  = 25.0 / 192.0;
        constexpr double MeterSpan  = 32.0 / 192.0;

        constexpr float BarHeight    = 8.f;
        constexpr float MarkerWidth  = 3.f;
        constexpr float MarkerHeight = 16.f;

        const sf::Color CoolColor = sf::Color(255, 200, 64);
        const sf::Color GoodColor = sf::Color(88, 148, 255);
        const sf::Color BadColor  = sf::Color(168, 96, 224);
        const sf::Color MissColor = sf::Color(110, 110, 110);

        sf::Color AccuracyColor(const Cx::Accuracy accuracy)
        {
            switch (accuracy)
            {
                case Cx::Accuracy::Cool: return CoolColor;
                case Cx::Accuracy::Good: return GoodColor;
                case Cx::Accuracy::Bad:  return BadColor;
                default:                 return MissColor;
            }
        }
    }

    AccuracyMeterExtension::AccuracyMeterExtension(Gx::EventDispatcher& events) :
        m_events(events)
    {
    }

    bool AccuracyMeterExtension::Initialize()
    {
        if (!StateExtension::Initialize())
            return false;

        BuildMeter();
        if (m_marker)
        {
            m_subscriptions.Add(m_events.On(Cx::PlayingEvents::OnJudgement, [this] (Cx::StatePlaying7K& sender, Cx::PlayingJudgementEventArgs& ev)
            {
                OnJudgement(ev);
            }));
        }

        return true;
    }

    void AccuracyMeterExtension::BuildMeter()
    {
        auto& state = GetState();

        const auto chatInput = Ui::FindDescendant<Gx::InputField>(state, Resource::ChatPanel::IDC_EDIT_CHAT);
        if (!chatInput || !chatInput->GetFont())
        {
            Log("AccuracyMeterMod: font source not found, meter skipped");
            return;
        }

        auto laneLeft  = 0.f;
        auto laneRight = 0.f;
        if (const auto keyContainer = state.FindChild<Gx::UiContainer>(Resource::Playing7K::IDC_CONTAINER_KEY_DOWN))
        {
            bool first = true;
            for (const auto child : keyContainer->GetChildren())
            {
                const auto image = dynamic_cast<Gx::Image*>(child);
                if (!image)
                    continue;

                const auto position = Ui::GlobalPosition(*image);
                const auto width    = image->GetLocalBounds().size.x;

                laneLeft  = first ? position.x : std::min(laneLeft, position.x);
                laneRight = first ? position.x + width : std::max(laneRight, position.x + width);
                first     = false;
            }
        }

        m_center   = sf::Vector2f(state.GetView().getSize().x / 2.f, state.GetView().getSize().y * 0.55f);
        m_barWidth = 224.f;
        if (laneRight > laneLeft)
        {
            m_center.x = (laneLeft + laneRight) / 2.f;
            m_barWidth = (laneRight - laneLeft) * 0.7f;
        }

        if (const auto judgement = Ui::FindByIdentifier<Gx::Node>(state, Resource::Playing7K::IDC_ANIMATION_NOTE_GOOD))
            m_center.y = Ui::GlobalPosition(*judgement).y + 56.f;
        else if (const auto comboNumber = Ui::FindByIdentifier<Gx::Node>(state, Resource::Playing7K::IDC_NUMBER_NOTE_COMBO))
            m_center.y = Ui::GlobalPosition(*comboNumber).y + 106.f;

        const auto addRegion = [&] (const double window, const sf::Color& color)
        {
            const auto width = static_cast<float>(window / MeterSpan) * m_barWidth;
            auto region      = std::make_unique<Gx::Rectangle>(sf::Vector2f(width, BarHeight));

            region->SetColor(color);
            region->SetPosition(m_center.x - width / 2.f, m_center.y - BarHeight / 2.f);

            state.AddChild(*region);
            m_parts.push_back(std::move(region));
        };

        addRegion(MeterSpan, MissColor);
        addRegion(BadWindow, BadColor);
        addRegion(GoodWindow, GoodColor);
        addRegion(CoolWindow, CoolColor);

        auto marker = std::make_unique<Gx::Rectangle>(sf::Vector2f(MarkerWidth, MarkerHeight));
        marker->SetColor(sf::Color::White);
        marker->SetPosition(m_center.x - MarkerWidth / 2.f, m_center.y - MarkerHeight / 2.f);
        m_marker = marker.get();

        state.AddChild(*marker);
        m_parts.push_back(std::move(marker));

        m_textTop = m_center.y + MarkerHeight / 2.f + 2.f;

        auto text = std::make_unique<Gx::Label>();
        text->SetName("O2CX_LATENCY_TEXT");
        text->SetFont(*chatInput->GetFont());
        text->SetCharacterSize(chatInput->GetCharacterSize());
        text->SetColor(MissColor);
        text->SetPosition(m_center.x, m_textTop);
        m_latencyText = text.get();

        state.AddChild(*text);
        m_parts.push_back(std::move(text));

        Log("AccuracyMeterMod: meter attached");
    }

    void AccuracyMeterExtension::OnJudgement(const Cx::PlayingJudgementEventArgs& ev)
    {
        const auto latency = ev.RenderPosition - ev.Note.Position;
        const auto offset  = static_cast<float>(std::clamp(latency, -MeterSpan, MeterSpan) / MeterSpan) * (m_barWidth / 2.f);

        m_marker->SetPosition(m_center.x + offset - MarkerWidth / 2.f, m_center.y - MarkerHeight / 2.f);

        m_latencyText->SetString(fmt::format("{:+}", static_cast<int>(std::lround(latency * 192.0))));
        m_latencyText->SetColor(AccuracyColor(ev.Accuracy));

        const auto bounds = m_latencyText->GetLocalBounds();
        m_latencyText->SetPosition(m_center.x - bounds.position.x - bounds.size.x / 2.f, m_textTop);
    }
}
