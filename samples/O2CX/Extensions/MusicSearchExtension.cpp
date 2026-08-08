#include <Extensions/MusicSearchExtension.hpp>
#include <Log.hpp>
#include <Ui.hpp>

#include <CXO2/Constants/Identifiers/ChatPanel.hpp>
#include <CXO2/Constants/Identifiers/SelectMusic.hpp>
#include <CXO2/Events/SelectMusicEvents.hpp>
#include <CXO2/UI/Dialogs/SelectMusicDialog.hpp>

#include <Genode/UI/List.hpp>

#include <vector>

namespace O2CX
{
    using namespace Cx::Constants::Identifiers;

    MusicSearchExtension::MusicSearchExtension(Gx::EventDispatcher& events)
    {
        m_subscriptions.Add(events.On(Cx::SelectMusicEvents::OnInitialize, [this] (Cx::SelectMusicDialog& sender, Cx::SelectMusicEventArgs& ev)
        {
            InjectSearchBox(sender);
        }));

        m_subscriptions.Add(events.On(Cx::SelectMusicEvents::OnDismiss, [this] (Cx::SelectMusicDialog& sender, Cx::SelectMusicDismissEventArgs& ev)
        {
            GetState().Invoke([this, &sender]
            {
                if (m_searchBox)
                    m_searchBox->SetString(sf::String());

                m_lastQuery = sf::String();
                sender.SetFilteredList(sender.GetMusicList());
            });
        }));
    }

    void MusicSearchExtension::Update(const sf::Time& delta)
    {
        StateExtension::Update(delta);

        if (!m_searchBox || !m_dialog)
            return;

        if (m_searchBox->GetString() == m_lastQuery)
            return;

        m_lastQuery = m_searchBox->GetString();
        ApplyFilter();
    }

    void MusicSearchExtension::ApplyFilter()
    {
        if (!m_dialog)
            return;

        const auto query = Ui::ToLower(m_lastQuery);
        if (query.isEmpty())
        {
            m_dialog->SetFilteredList(m_dialog->GetMusicList());
            return;
        }

        auto filtered = std::vector<Cx::ChartMetadata>();
        for (const auto& entry : m_dialog->GetMusicList())
        {
            if (Ui::ToLower(entry.Title).find(query) != sf::String::InvalidPos)
                filtered.push_back(entry);
        }

        m_dialog->SetFilteredList(filtered);
    }

    void MusicSearchExtension::InjectSearchBox(Cx::SelectMusicDialog& dialog)
    {
        if (m_searchBox)
            return;

        m_dialog = &dialog;
        GetState().Invoke([this, &dialog]
        {
            if (m_searchBox)
                return;

            const auto infoList  = dialog.FindChild<Gx::List>(Resource::SelectMusic::IDC_LIST_MUSIC_INFO);
            const auto chatInput = Ui::FindDescendant<Gx::InputField>(GetState(), Resource::ChatPanel::IDC_EDIT_CHAT);
            if (!infoList || !chatInput || !chatInput->GetFont())
            {
                Log("MusicSearchMod: prerequisites not found, search box skipped");
                return;
            }

            const auto rows      = infoList->GetChildren();
            const auto bpmRow    = rows.size() > 4 ? dynamic_cast<Gx::Label*>(rows[4]) : nullptr;
            const auto noteRow   = rows.size() > 3 ? dynamic_cast<Gx::Label*>(rows[3]) : nullptr;
            if (!bpmRow || !noteRow)
                return;

            constexpr unsigned int characterSize = 10;
            const auto outlineColor              = sf::Color(25, 25, 25);
            constexpr float outlineThickness     = 1.25f;

            const auto font     = bpmRow->GetFont() ? bpmRow->GetFont() : chatInput->GetFont();
            const auto color    = bpmRow->GetColor();
            const auto rowStep  = Ui::GlobalPosition(*bpmRow) - Ui::GlobalPosition(*noteRow);
            auto position = Ui::GlobalPosition(*bpmRow) + rowStep - Ui::GlobalPosition(dialog);
            position.x += 3;

            m_searchLabel = std::make_unique<Gx::Label>();
            m_searchLabel->SetName("O2CX_SEARCH_LABEL");
            m_searchLabel->SetFont(*font);
            m_searchLabel->SetCharacterSize(characterSize);
            m_searchLabel->SetColor(color);
            m_searchLabel->SetOutlineColor(outlineColor);
            m_searchLabel->SetOutlineThickness(outlineThickness);
            m_searchLabel->SetOutlineOffset({0.f, 1.f});
            m_searchLabel->SetString("Search : ");
            m_searchLabel->SetPosition(position);

            const auto labelBounds = m_searchLabel->GetLocalBounds();

            m_searchBox = std::make_unique<Gx::InputField>(*font, sf::String(), characterSize, sf::FloatRect{{0.f, 0.f}, {150.f, characterSize + 4.f}});
            m_searchBox->SetName("O2CX_SEARCH_BOX");
            m_searchBox->SetColor(color);
            m_searchBox->SetOutlineColor(outlineColor);
            m_searchBox->SetOutlineThickness(outlineThickness);
            m_searchBox->SetOutlineOffset({0.f, 1.f});
            m_searchBox->SetMaximumTextLength(24);
            m_searchBox->SetPosition(position + sf::Vector2f(labelBounds.size.x + 4.f, 0.f));

            dialog.AddChild(*m_searchLabel);
            dialog.AddChild(*m_searchBox);

            Log("MusicSearchMod: search box injected");
        });
    }
}
